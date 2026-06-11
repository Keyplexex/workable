#include "memory.hpp"
#include <cstring>
#include <iostream>

Memory::Memory() : ram_(MEMORY_SIZE, 0) {}

void Memory::load_frag(uint32_t addr, const std::vector<uint8_t>& data) {
    if (addr + data.size() > ram_.size()) return;
    std::memcpy(&ram_[addr], data.data(), data.size());
}

bool Memory::read_u8(uint32_t addr, uint8_t& v) const {
    if (addr >= ram_.size()) return false; v = ram_[addr]; return true;
}
bool Memory::read_u16(uint32_t addr, uint16_t& v) const {
    if (addr+1 >= ram_.size()) return false;
    v = static_cast<uint16_t>(ram_[addr]) | (static_cast<uint16_t>(ram_[addr+1])<<8);
    return true;
}
bool Memory::read_u32(uint32_t addr, uint32_t& v) const {
    if (addr+3 >= ram_.size()) return false;
    v =  static_cast<uint32_t>(ram_[addr])
       | (static_cast<uint32_t>(ram_[addr+1])<<8)
       | (static_cast<uint32_t>(ram_[addr+2])<<16)
       | (static_cast<uint32_t>(ram_[addr+3])<<24);
    return true;
}

bool Memory::write_u8(uint32_t addr, uint8_t val) {
    if (addr >= ram_.size()) return false; ram_[addr]=val; return true;
}
bool Memory::write_u16(uint32_t addr, uint16_t v) {
    if (addr+1 >= ram_.size()) return false;
    ram_[addr]   = static_cast<uint8_t>(v & 0xFF);
    ram_[addr+1] = static_cast<uint8_t>((v>>8) & 0xFF);
    return true;
}
bool Memory::write_u32(uint32_t addr, uint32_t v) {
    if (addr+3 >= ram_.size()) return false;
    ram_[addr]   = static_cast<uint8_t>(v & 0xFF);
    ram_[addr+1] = static_cast<uint8_t>((v>>8) & 0xFF);
    ram_[addr+2] = static_cast<uint8_t>((v>>16)& 0xFF);
    ram_[addr+3] = static_cast<uint8_t>((v>>24)& 0xFF);
    return true;
}
