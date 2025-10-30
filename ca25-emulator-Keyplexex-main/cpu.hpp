#pragma once
#include <cstdint>
#include "memory.hpp"
#include "cache.hpp"

struct ExecResult {
    bool ok = true;
    bool halted = false;
    uint32_t final_pc = 0;
};

class CPU {
public:
    CPU();
    void reset_from_regs(const uint32_t regs_in[32], uint32_t& start_ra_out);
    void export_regs(uint32_t regs_out[32]) const;

    ExecResult run(Memory& mem, Cache& cache_lru, Cache& cache_bplru,
                   bool enable_bplru, uint32_t start_ra);

//private:
    uint32_t x_[32]{};
    uint32_t pc_{0};
	uint32_t get_pc() const;
};
