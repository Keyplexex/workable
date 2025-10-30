#pragma once
#include <cstdint>
#include "config.hpp"

// Тип обращения для раздельной статистики
enum class AccessKind { Inst, Data };

// Счётчики попаданий/промахов
struct CacheStats {
    uint64_t hits_total = 0,  misses_total = 0;
    uint64_t hits_inst  = 0,  misses_inst  = 0;
    uint64_t hits_data  = 0,  misses_data  = 0;
};

// Интерфейс кэша 
class Cache {
public:
    virtual ~Cache() = default;
    virtual bool read (uint32_t addr, std::size_t size, AccessKind k) = 0; // только моделирование попадания/промаха
    virtual bool write(uint32_t addr, std::size_t size, AccessKind k) = 0;
    const CacheStats& stats() const { return st_; }
protected:
    CacheStats st_;
};

Cache* make_lru_cache();
Cache* make_bplru_cache();
