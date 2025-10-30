// src/cpu.cpp
#include "cpu.hpp"
#include <cstring>
#include <limits>
#include <cstdint>
#include <iostream>

// ---- helpers ----
static inline uint32_t get_bits(uint32_t x, int hi, int lo) {
    return (x >> lo) & ((1u << (hi - lo + 1)) - 1);
}

// sign-extend 'bits' LSBs of x into 32-bit signed value, returned as uint32_t bitpattern
static inline uint32_t sext(uint32_t x, int bits) {
    uint32_t m = 1u << (bits - 1);
    return (x ^ m) - m;
}

static inline int32_t imm_i_s(int32_t insn) { return static_cast<int32_t>(sext((uint32_t)get_bits((uint32_t)insn,31,20), 12)); }
static inline uint32_t imm_i(uint32_t insn) { return sext(get_bits(insn,31,20), 12); }
static inline uint32_t imm_s(uint32_t insn) {
    uint32_t v = (get_bits(insn,31,25) << 5) | get_bits(insn,11,7);
    return sext(v, 12);
}
static inline uint32_t imm_b(uint32_t insn) {
    uint32_t v = (get_bits(insn,31,31) << 12)
               | (get_bits(insn,7,7)   << 11)
               | (get_bits(insn,30,25) << 5)
               | (get_bits(insn,11,8)  << 1);
    return sext(v, 13);
}
static inline uint32_t imm_u(uint32_t insn) { return insn & 0xFFFFF000u; }
static inline uint32_t imm_j(uint32_t insn) {
    uint32_t v = (get_bits(insn,31,31) << 20)
               | (get_bits(insn,19,12) << 12)
               | (get_bits(insn,20,20) << 11)
               | (get_bits(insn,30,21) << 1);
    return sext(v, 21);
}

CPU::CPU() {
    std::memset(x_, 0, sizeof(x_));
}

uint32_t CPU::get_pc() const {
    return pc_;
}

void CPU::reset_from_regs(const uint32_t regs_in[32], uint32_t& start_ra_out) {
    // regs_in[0] is PC, regs_in[1..31] are x1..x31
    pc_ = regs_in[0];
    x_[0] = 0;
    for (int i = 1; i < 32; ++i) x_[i] = regs_in[i];
    start_ra_out = x_[1]; // ra at start
}

void CPU::export_regs(uint32_t regs_out[32]) const {
    regs_out[0] = pc_ + 4;
    for (int i = 1; i < 32; ++i) regs_out[i] = x_[i];
}


static inline void write_rd(uint32_t* x, int rd, uint32_t val) {
    if (rd != 0) x[rd] = val;
}

ExecResult CPU::run(Memory& mem, Cache& cache_lru, Cache& cache_bplru,
                    bool enable_bplru, uint32_t start_ra)
{
    ExecResult r;
    r.ok = true;
    r.halted = false;
    r.final_pc = pc_;

    const uint64_t MAX_STEPS = 50'000'000ULL;
    uint64_t steps = 0;

    // Fetch 32-bit instruction (with cache accesses)
    auto do_fetch32 = [&](uint32_t addr, uint32_t &instr)->bool {
        cache_lru.read(addr, 4, AccessKind::Inst);
        if (enable_bplru) cache_bplru.read(addr, 4, AccessKind::Inst);
        return mem.read_u32(addr, instr);
    };

    // Load/store helpers go through caches first (to collect stats), then memory
    auto do_load = [&](uint32_t addr, int size, uint32_t &out)->bool {
        cache_lru.read(addr, size, AccessKind::Data);
        if (enable_bplru) cache_bplru.read(addr, size, AccessKind::Data);
        switch (size) {
            case 1: { uint8_t v; if (!mem.read_u8(addr, v)) return false; out = v; return true; }
            case 2: { uint16_t v; if (!mem.read_u16(addr, v)) return false; out = v; return true; }
            case 4: { uint32_t v; if (!mem.read_u32(addr, v)) return false; out = v; return true; }
            default: return false;
        }
    };

    auto do_store = [&](uint32_t addr, int size, uint32_t val)->bool {
        cache_lru.write(addr, size, AccessKind::Data);
        if (enable_bplru) cache_bplru.write(addr, size, AccessKind::Data);
        switch (size) {
            case 1: return mem.write_u8(addr, static_cast<uint8_t>(val));
            case 2: return mem.write_u16(addr, static_cast<uint16_t>(val));
            case 4: return mem.write_u32(addr, val);
            default: return false;
        }
    };

    auto set_pc_and_check_return = [&](uint32_t new_pc)->bool {
        pc_ = new_pc;
        if (steps > 0 && pc_ == start_ra) {
            r.halted = true;
            r.final_pc = pc_;
            return true;
        }
        return false;
    };

    while (steps++ < MAX_STEPS) {
        uint32_t instr;
        if (!do_fetch32(pc_, instr)) { r.ok = false; return r; }

        const uint32_t opcode = get_bits(instr, 6, 0);
        const int rd = (int)get_bits(instr, 11, 7);
        const int funct3 = (int)get_bits(instr, 14, 12);
        const int rs1 = (int)get_bits(instr, 19, 15);
        const int rs2 = (int)get_bits(instr, 24, 20);
        const uint32_t funct7 = get_bits(instr, 31, 25);

        uint32_t next_pc = pc_ + 4;

        switch (opcode) {
            case 0x37: // LUI
                write_rd(x_, rd, imm_u(instr));
                if (set_pc_and_check_return(next_pc)) return r;
                break;

            case 0x17: // AUIPC
                write_rd(x_, rd, pc_ + imm_u(instr));
                if (set_pc_and_check_return(next_pc)) return r;
                break;

            case 0x6F: { // JAL
                uint32_t ret = pc_ + 4;
                uint32_t target = pc_ + imm_j(instr);
                write_rd(x_, rd, ret);
                if (set_pc_and_check_return(target)) return r;
                break;
            }

            case 0x67: { // JALR
                uint32_t ret = pc_ + 4;
                uint32_t target = (x_[rs1] + imm_i(instr)) & ~1u;
                write_rd(x_, rd, ret);
                if (set_pc_and_check_return(target)) return r;
                break;
            }

            case 0x63: { // BRANCH
                int32_t off = static_cast<int32_t>(imm_b(instr));
                uint32_t a = x_[rs1], b = x_[rs2];
                bool take = false;
                switch (funct3) {
                    case 0b000: take = (a == b); break;                // BEQ
                    case 0b001: take = (a != b); break;                // BNE
                    case 0b100: take = (static_cast<int32_t>(a) < static_cast<int32_t>(b)); break; // BLT
                    case 0b101: take = (static_cast<int32_t>(a) >= static_cast<int32_t>(b)); break; // BGE
                    case 0b110: take = (a < b); break;                 // BLTU
                    case 0b111: take = (a >= b); break;                // BGEU
                    default: r.ok = false; return r;
                }
                if (take) {
                    if (set_pc_and_check_return(static_cast<uint32_t>(pc_ + off))) return r;
                } else {
                    if (set_pc_and_check_return(next_pc)) return r;
                }
                break;
            }

            case 0x03: { // LOAD
                uint32_t addr = x_[rs1] + imm_i(instr);
                uint32_t tmp = 0;
                bool ok = false;
                switch (funct3) {
                    case 0b000: ok = do_load(addr, 1, tmp); if (ok) write_rd(x_, rd, sext(tmp, 8)); break;   // LB
                    case 0b001: ok = do_load(addr, 2, tmp); if (ok) write_rd(x_, rd, sext(tmp, 16)); break;  // LH
                    case 0b010: ok = do_load(addr, 4, tmp); if (ok) write_rd(x_, rd, tmp); break;            // LW
                    case 0b100: ok = do_load(addr, 1, tmp); if (ok) write_rd(x_, rd, tmp & 0xFFu); break;    // LBU
                    case 0b101: ok = do_load(addr, 2, tmp); if (ok) write_rd(x_, rd, tmp & 0xFFFFu); break;  // LHU
                    default: r.ok = false; return r;
                }
                if (!ok) { r.ok = false; return r; }
                if (set_pc_and_check_return(next_pc)) return r;
                break;
            }

            case 0x23: { // STORE
                uint32_t addr = x_[rs1] + imm_s(instr);
                uint32_t val = x_[rs2];
                bool ok = false;
                switch (funct3) {
                    case 0b000: ok = do_store(addr, 1, val); break; // SB
                    case 0b001: ok = do_store(addr, 2, val); break; // SH
                    case 0b010: ok = do_store(addr, 4, val); break; // SW
                    default: r.ok = false; return r;
                }
                if (!ok) { r.ok = false; return r; }
                if (set_pc_and_check_return(next_pc)) return r;
                break;
            }

            case 0x13: { // I-type ALU
                uint32_t imm = imm_i(instr);
                switch (funct3) {
                    case 0b000: write_rd(x_, rd, x_[rs1] + imm); break;                       // ADDI
                    case 0b010: write_rd(x_, rd, (int32_t)x_[rs1] < (int32_t)imm); break;     // SLTI
                    case 0b011: write_rd(x_, rd, x_[rs1] < imm); break;                      // SLTIU
                    case 0b100: write_rd(x_, rd, x_[rs1] ^ imm); break;                      // XORI
                    case 0b110: write_rd(x_, rd, x_[rs1] | imm); break;                      // ORI
                    case 0b111: write_rd(x_, rd, x_[rs1] & imm); break;                      // ANDI
                    case 0b001: { // SLLI
                        if (get_bits(instr, 31, 25) != 0) { r.ok = false; return r; }
                        write_rd(x_, rd, x_[rs1] << get_bits(instr, 24, 20));
                        break;
                    }
                    case 0b101: { // SRLI / SRAI
                        uint32_t shamt = get_bits(instr, 24, 20);
                        if (get_bits(instr, 31, 25) == 0b0000000)
                            write_rd(x_, rd, x_[rs1] >> shamt); // SRLI
                        else if (get_bits(instr, 31, 25) == 0b0100000)
                            write_rd(x_, rd, static_cast<uint32_t>(static_cast<int32_t>(x_[rs1]) >> shamt)); // SRAI
                        else { r.ok = false; return r; }
                        break;
                    }
                    default: r.ok = false; return r;
                }
                if (set_pc_and_check_return(next_pc)) return r;
                break;
            }

            case 0x33: { // R-type ALU / RV32M
                if (funct7 == 0b0000001) {
                    // RV32M extension
                    switch (funct3) {
                        case 0b000: { // MUL
                            int64_t prod = (int64_t)(int32_t)x_[rs1] * (int64_t)(int32_t)x_[rs2];
                            write_rd(x_, rd, static_cast<uint32_t>(prod & 0xFFFFFFFFu));
                            break;
                        }
                        case 0b001: { // MULH
                            int64_t prod = (int64_t)(int32_t)x_[rs1] * (int64_t)(int32_t)x_[rs2];
                            write_rd(x_, rd, static_cast<uint32_t>((prod >> 32) & 0xFFFFFFFFu));
                            break;
                        }
                        case 0b010: { // MULHSU
                            int64_t prod = (int64_t)(int32_t)x_[rs1] * (uint64_t)x_[rs2];
                            write_rd(x_, rd, static_cast<uint32_t>((prod >> 32) & 0xFFFFFFFFu));
                            break;
                        }
                        case 0b011: { // MULHU
                            uint64_t prod = (uint64_t)x_[rs1] * (uint64_t)x_[rs2];
                            write_rd(x_, rd, static_cast<uint32_t>((prod >> 32) & 0xFFFFFFFFu));
                            break;
                        }
                        case 0b100: { // DIV
                            int32_t a = static_cast<int32_t>(x_[rs1]);
                            int32_t b = static_cast<int32_t>(x_[rs2]);
                            if (b == 0) write_rd(x_, rd, 0xFFFFFFFFu);
                            else if (a == INT32_MIN && b == -1) write_rd(x_, rd, static_cast<uint32_t>(INT32_MIN));
                            else write_rd(x_, rd, static_cast<uint32_t>(a / b));
                            break;
                        }
                        case 0b101: { // DIVU
                            uint32_t a = x_[rs1], b = x_[rs2];
                            write_rd(x_, rd, b == 0 ? 0xFFFFFFFFu : a / b);
                            break;
                        }
                        case 0b110: { // REM
                            int32_t a = static_cast<int32_t>(x_[rs1]);
                            int32_t b = static_cast<int32_t>(x_[rs2]);
                            if (b == 0) write_rd(x_, rd, static_cast<uint32_t>(a));
                            else if (a == INT32_MIN && b == -1) write_rd(x_, rd, 0u);
                            else write_rd(x_, rd, static_cast<uint32_t>(a % b));
                            break;
                        }
                        case 0b111: { // REMU
                            uint32_t a = x_[rs1], b = x_[rs2];
                            write_rd(x_, rd, b == 0 ? a : a % b);
                            break;
                        }
                        default: r.ok = false; return r;
                    }
                } else {
                    // RV32I R-type
                    switch ((funct7 << 3) | funct3) {
                        case (0b0000000 << 3) | 0b000: write_rd(x_, rd, x_[rs1] + x_[rs2]); break; // ADD
                        case (0b0100000 << 3) | 0b000: write_rd(x_, rd, x_[rs1] - x_[rs2]); break; // SUB
                        case (0b0000000 << 3) | 0b111: write_rd(x_, rd, x_[rs1] & x_[rs2]); break; // AND
                        case (0b0000000 << 3) | 0b110: write_rd(x_, rd, x_[rs1] | x_[rs2]); break; // OR
                        case (0b0000000 << 3) | 0b100: write_rd(x_, rd, x_[rs1] ^ x_[rs2]); break; // XOR
                        case (0b0000000 << 3) | 0b001: write_rd(x_, rd, x_[rs1] << (x_[rs2] & 31)); break; // SLL
                        case (0b0000000 << 3) | 0b101: write_rd(x_, rd, x_[rs1] >> (x_[rs2] & 31)); break; // SRL
                        case (0b0100000 << 3) | 0b101: write_rd(x_, rd, static_cast<uint32_t>(static_cast<int32_t>(x_[rs1]) >> (x_[rs2] & 31))); break; // SRA
                        case (0b0000000 << 3) | 0b010: write_rd(x_, rd, (int32_t)x_[rs1] < (int32_t)x_[rs2]); break; // SLT
                        case (0b0000000 << 3) | 0b011: write_rd(x_, rd, x_[rs1] < x_[rs2]); break; // SLTU
                        default: r.ok = false; return r;
                    }
                }
                if (set_pc_and_check_return(next_pc)) return r;
                break;
            }

            case 0x73: { // SYSTEM (ecall / ebreak)
                uint32_t csr_imm = get_bits(instr, 31, 20);
                if (funct3 == 0 && (csr_imm == 0 || csr_imm == 1)) {
                    r.halted = true;
                    r.final_pc = pc_;
                    return r;
                } else {
                    r.ok = false;
                    return r;
                }
            }

            default:
                r.ok = false;
                return r;
        }

        // ensure x0 == 0
        x_[0] = 0;
    }

    // exceeded MAX_STEPS
    r.ok = false;
    return r;
}
