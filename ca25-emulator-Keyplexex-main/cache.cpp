#include "cache.hpp"
#include "config.hpp"
#include <vector>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <cassert>

namespace {

struct Line {
    bool     valid{false};
    bool     dirty{false};
    uint32_t tag{0};
    uint8_t  age{0};
    bool plru = false;
};

struct Set {
    Line ways[CACHE_WAY];
    // bits[0] — корень (0->лево, 1->право), bits[1] — узел левого поддерева (0->лево(way0),1->право(way1)),
    // bits[2] — узел правого поддерева (0->лево(way2),1->право(way3)).
    // uint8_t plru_bits{0}; // используем младшие 3 бита
};

inline uint32_t addr_index(uint32_t addr) {
    return (addr >> CACHE_OFFSET_LEN) & ((1u << CACHE_INDEX_LEN) - 1u);
}
inline uint32_t addr_tag(uint32_t addr) {
    return addr >> (CACHE_OFFSET_LEN + CACHE_INDEX_LEN);
}

// --- базовый класс с общим хелпером обновления статистики ---
class BaseCache : public Cache {
protected:
    std::vector<Set> sets_;

    // counters for accesses separated by kind (instruction / data)
    uint64_t total_accesses_inst_ = 0;
    uint64_t total_accesses_data_ = 0;

    void stat_hit(AccessKind k) {
        st_.hits_total++;
        if (k == AccessKind::Inst) st_.hits_inst++; else st_.hits_data++;
    }
    void stat_miss(AccessKind k) {
        st_.misses_total++;
        if (k == AccessKind::Inst) st_.misses_inst++; else st_.misses_data++;
    }

    void count_access(AccessKind k) {
        if (k == AccessKind::Inst) ++total_accesses_inst_;
        else ++total_accesses_data_;
    }

public:
    BaseCache() : sets_(CACHE_SET_COUNT) {}

    // Accessors useful for debug/comparison with другой реализацией
    uint64_t total_inst_accesses() const { return total_accesses_inst_; }
    uint64_t total_data_accesses() const { return total_accesses_data_; }
};

// ---------------- LRU ----------------
class LRUCache : public BaseCache {
    // Возвращает индекс way при попадании, иначе -1
    int find_hit(uint32_t set_idx, uint32_t tag) {
        auto& S = sets_[set_idx];
        for (int w=0; w<(int)CACHE_WAY; ++w) {
            if (S.ways[w].valid && S.ways[w].tag == tag) return w;
        }
        return -1;
    }

    // Выбор жертвы: сначала ищем invalid; иначе — максимальный age
    int victim_lru(uint32_t set_idx) {
        auto& S = sets_[set_idx];
        // сначала свободная линия
        for (int w=0; w<(int)CACHE_WAY; ++w) if (!S.ways[w].valid) return w;
        // иначе — выбрать максимальную age
        int vw = 0;
        uint8_t best = S.ways[0].age;
        for (int w=1; w<(int)CACHE_WAY; ++w) {
            if (S.ways[w].age > best) { best = S.ways[w].age; vw = w; }
        }
        return vw;
    }

    // Обновить LRU-возраст: у попавшей линии age=0; у остальных в наборе +1 (с насыщением до 255)
    void touch_lru(uint32_t set_idx, int used_way) {
        auto& S = sets_[set_idx];
        for (int w=0; w<(int)CACHE_WAY; ++w) {
            if (w == used_way) S.ways[w].age = 0;
            else if (S.ways[w].valid && S.ways[w].age < 255) S.ways[w].age++;
        }
    }

    // «Загрузка» в жертву: выставляем метаданные; dirty выставляем по типу операции
    void fill_line(uint32_t set_idx, int way, uint32_t tag, bool dirty_on_fill) {
        auto& L = sets_[set_idx].ways[way];
        L.valid = true;
        L.tag   = tag;
        L.dirty = dirty_on_fill;
        L.age   = 0;
        // Остальным age++ сделаем через touch_lru (вызовем сразу после fill)
    }

public:
    bool read (uint32_t addr, std::size_t /*size*/, AccessKind k) override {
        count_access(k); // важный счётчик
        const uint32_t si  = addr_index(addr);
        const uint32_t tag = addr_tag(addr);
        int w = find_hit(si, tag);
        if (w >= 0) { stat_hit(k); touch_lru(si, w); return true; }
        stat_miss(k);
        int v = victim_lru(si);
        // write-back: если dirty — «сбрасываем» (в нашей модели это no-op)
        // write-allocate: при чтении всегда аллоцируем и помечаем clean
        fill_line(si, v, tag, /*dirty_on_fill=*/false);
        touch_lru(si, v);
        return false;
    }

    bool write(uint32_t addr, std::size_t /*size*/, AccessKind k) override {
        count_access(k); // важный счётчик
        const uint32_t si  = addr_index(addr);
        const uint32_t tag = addr_tag(addr);
        int w = find_hit(si, tag);
        if (w >= 0) { // hit
            stat_hit(k);
            sets_[si].ways[w].dirty = true; // write-back
            touch_lru(si, w);
            return true;
        }
        // miss
        stat_miss(k);
        int v = victim_lru(si);
        // если была грязная — «сбросить» (no-op)
        // write-allocate: загружаем и сразу помечаем dirty
        fill_line(si, v, tag, /*dirty_on_fill=*/true);
        touch_lru(si, v);
        return false;
    }
};

// ---------------- (bpLRU) ----------------
class BpLRUCache : public BaseCache {
    // Поиск попадания
    int find_hit(uint32_t set_idx, uint32_t tag) {
        auto& S = sets_[set_idx];
        for (int w = 0; w < (int)CACHE_WAY; ++w) {
            if (S.ways[w].valid && S.ways[w].tag == tag) return w;
        }
        return -1;
    }


void touch_bplru(uint32_t set_idx, int way) {
    auto& S = sets_[set_idx];
    S.ways[way].plru = true;

    // Проверяем, все ли стали 1
    bool all_one = true;
    for (int w = 0; w < (int)CACHE_WAY; ++w) {
        if (!S.ways[w].plru) {
            all_one = false;
            break;
        }
    }

    if (all_one) {
        // Обнуляем все, кроме текущей
        for (int w = 0; w < (int)CACHE_WAY; ++w) {
            if (w != way) S.ways[w].plru = false;
        }
    }
}

int victim_bplru(uint32_t set_idx) {
    auto& S = sets_[set_idx];

    // Сначала ищем invalid
    for (int w = 0; w < (int)CACHE_WAY; ++w)
        if (!S.ways[w].valid) return w;

    // Ищем первую линию с plru = 0
    for (int w = 0; w < (int)CACHE_WAY; ++w)
        if (!S.ways[w].plru) return w;

    return 0;
}




    void fill_line(uint32_t set_idx, int way, uint32_t tag, bool dirty_on_fill) {
        auto& L = sets_[set_idx].ways[way];
        L.valid = true;
        L.tag   = tag;
        L.dirty = dirty_on_fill;
        L.age   = 0;
        touch_bplru(set_idx, way);
    }

public:
    bool read(uint32_t addr, std::size_t /*size*/, AccessKind k) override {
        count_access(k);
        const uint32_t si  = addr_index(addr);
        const uint32_t tag = addr_tag(addr);
        int w = find_hit(si, tag);
        if (w >= 0) { 
            stat_hit(k); 
            touch_bplru(si, w); 
            return true; 
        }
        stat_miss(k);
        int v = victim_bplru(si);
        fill_line(si, v, tag, /*dirty_on_fill=*/false);
        return false;
    }
    
    bool write(uint32_t addr, std::size_t /*size*/, AccessKind k) override {
        count_access(k);
        const uint32_t si  = addr_index(addr);
        const uint32_t tag = addr_tag(addr);
        int w = find_hit(si, tag);
        if (w >= 0) {
            stat_hit(k);
            sets_[si].ways[w].dirty = true;
            touch_bplru(si, w);
            return true;
        }
        stat_miss(k);
        int v = victim_bplru(si);
        fill_line(si, v, tag, /*dirty_on_fill=*/true);
        return false;
    }
};
} 

Cache* make_lru_cache()   { return new LRUCache(); }
Cache* make_bplru_cache() { return new BpLRUCache(); }
