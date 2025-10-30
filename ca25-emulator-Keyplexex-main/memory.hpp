#pragma once
#include <cstdint>
#include <vector>
#include "config.hpp"

class Memory {
public:
    Memory(); 
    void load_frag(uint32_t addr, const std::vector<uint8_t>& data);

    // Низкоуровневые чтения/записи
    bool read_u8 (uint32_t addr, uint8_t&  v) const;
    bool read_u16(uint32_t addr, uint16_t& v) const;
    bool read_u32(uint32_t addr, uint32_t& v) const;

    bool write_u8 (uint32_t addr, uint8_t  v);
    bool write_u16(uint32_t addr, uint16_t v);
    bool write_u32(uint32_t addr, uint32_t v);

    const std::vector<uint8_t>& raw() const { return ram_; }
          std::vector<uint8_t>& raw()       { return ram_; }

private:
    std::vector<uint8_t> ram_;
};
