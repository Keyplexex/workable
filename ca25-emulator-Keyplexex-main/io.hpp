#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

struct InputImage {
    uint32_t regs[32]; 
    struct Frag { uint32_t addr; std::vector<uint8_t> data; };
    std::vector<Frag> frags;
};

struct OutputImage {
    uint32_t regs[32];
    uint32_t start_addr = 0;
    std::vector<uint8_t> mem;
};

std::optional<InputImage> read_input_file(const std::string& path, std::string& err);
bool write_output_file(const std::string& path, const OutputImage& out, std::string& err);

bool parse_u32(const char* s, uint32_t& out);
