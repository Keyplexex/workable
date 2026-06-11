// main.cpp
#include <cstdio>
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <cmath>
#include <iostream>
#include "config.hpp"
#include "errors.hpp"
#include "io.hpp"
#include "memory.hpp"
#include "cache.hpp"
#include "cpu.hpp"

struct Args {
    std::string in_path;
    std::optional<std::string> out_path;
    std::optional<uint32_t> out_addr;
    std::optional<uint32_t> out_size;
};

bool parse_u32_safe(const char* s, uint32_t& out) {
    char* end = nullptr;
    unsigned long val = strtoul(s, &end, 0);
    if (*end != '\0' || val > UINT32_MAX) return false;
    out = static_cast<uint32_t>(val);
    return true;
}

static std::optional<Args> parse_args(int argc, char** argv, std::string& err) {
    Args a;
    for (int i = 1; i < argc; ++i) {
        std::string s = argv[i];
        if (s == "-i") {
            if (i + 1 >= argc) { err = "missing argument after -i"; return std::nullopt; }
            a.in_path = argv[++i];
        } else if (s == "-o") {
            if (i + 3 >= argc) { err = "missing arguments after -o"; return std::nullopt; }
            a.out_path = argv[++i];
            uint32_t addr, size;
            if (!parse_u32_safe(argv[++i], addr)) { err = "invalid output start address"; return std::nullopt; }
            if (!parse_u32_safe(argv[++i], size)) { err = "invalid output size"; return std::nullopt; }
            a.out_addr = addr;
            a.out_size = size;
        } else {
            err = "unknown argument: " + s;
            return std::nullopt;
        }
    }
    if (a.in_path.empty()) { err = "input file is required (-i <file>)"; return std::nullopt; }
    return a;
}

static void print_line_percent_or_unsupported(const char* name, const CacheStats& st, bool unsupported) {
    if (unsupported) {
        std::printf("%s\tunsupported\tunsupported\tunsupported\n", name);
        return;
    }
    auto pr = [&](uint64_t hits, uint64_t misses) {
        uint64_t total = hits + misses;
        if (total == 0) {
            std::printf("nan%%");
        } else {
            double rate = 100.0 * hits / total;
            std::printf("%3.5f%%", rate);
        }
    };
    std::printf("%s\t", name);
    pr(st.hits_total, st.misses_total);  std::printf("\t");
    pr(st.hits_inst , st.misses_inst );  std::printf("\t");
    pr(st.hits_data , st.misses_data );  std::printf("\n");
}

int main(int argc, char** argv) {
    std::string err;
    auto args = parse_args(argc, argv, err);
    if (!args) { std::fprintf(stderr, "Error parsing arguments: %s\n", err.c_str()); return 1; }

    auto img = read_input_file(args->in_path, err);
    if (!img) { std::fprintf(stderr, "Error reading input file: %s\n", err.c_str()); return 2; }

    // Load fragments into memory
    Memory mem;
    for (auto& f : img->frags) {
        if (static_cast<uint64_t>(f.addr) + f.data.size() > MEMORY_SIZE) {
            std::fprintf(stderr, "Fragment out of memory bounds\n"); return 3;
        }
        mem.load_frag(f.addr, f.data);
    }

    // Initialize CPU registers from image
    CPU cpu;
    uint32_t start_ra = 0;
    cpu.reset_from_regs(img->regs, start_ra);

    // Create caches
    std::unique_ptr<Cache> lru(make_lru_cache());
    std::unique_ptr<Cache> bplru(make_bplru_cache());

    // If bplru not provided by implementation, we'll print "unsupported" for it later
    bool bplru_unsupported = (bplru == nullptr);

    // Run CPU: caches will be used inside CPU::run (we pass both caches)
    ExecResult r = cpu.run(mem, *lru, (bplru ? *bplru : *lru), /*enable_bplru=*/!bplru_unsupported, start_ra);
    if (!r.ok) { std::fprintf(stderr, "Execution failed\n"); return 4; }

    // Print cache stats
    std::printf("replacement\thit rate\thit rate (inst)\thit rate (data)\n");
    print_line_percent_or_unsupported("        LRU",   lru->stats(), false);
    print_line_percent_or_unsupported("      bpLRU", bplru_unsupported ? CacheStats{} : bplru->stats(), bplru_unsupported);

    // If output requested, write registers+memory fragment as specified
    if (args->out_path) {
        OutputImage out{};
        cpu.export_regs(out.regs);

        // Получаем адрес и размер как 64-битные
        int64_t start_addr_signed = args->out_addr.value();
        int64_t size_signed       = args->out_size.value();

        // Проверка на отрицательные значения
        if (start_addr_signed < 0 || size_signed < 0) {
            std::fprintf(stderr, "Negative start address or size\n");
            return 5;
        }

        uint64_t start = static_cast<uint64_t>(start_addr_signed);
        uint64_t size  = static_cast<uint64_t>(size_signed);

        // Проверка на переполнение
        if (start > MEMORY_SIZE || size > MEMORY_SIZE || start + size > MEMORY_SIZE) {
            std::fprintf(stderr, "Output range out of memory bounds\n");
            return 5;
        }

        out.start_addr = start;

        // Безопасно копируем срез памяти
        auto &raw_mem = mem.raw(); // важно: должна быть ссылка!
        out.mem.assign(raw_mem.begin() + start,
                    raw_mem.begin() + start + size);

        // Пишем в файл
        if (!write_output_file(args->out_path.value(), out, err)) {
            std::fprintf(stderr, "Error writing output file: %s\n", err.c_str());
            return 6;
        }
    }


    return 0;
}
