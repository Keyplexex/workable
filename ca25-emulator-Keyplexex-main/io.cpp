#include "io.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>

static bool read_exact(FILE* f, void* buf, size_t n) {
    return std::fread(buf, 1, n, f) == n;
}
static bool write_exact(FILE* f, const void* buf, size_t n) {
    return std::fwrite(buf, 1, n, f) == n;
}

std::optional<InputImage> read_input_file(const std::string& path, std::string& err) {
    InputImage img{};
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) { err = "failed to open input file"; return std::nullopt; }

    if (!read_exact(f, img.regs, 32u*4u)) { err = "truncated header"; std::fclose(f); return std::nullopt; }

    // Читаем фрагменты: [addr:4][size:4][data:N] пока не EOF
    while (true) {
        uint32_t addr=0, size=0;
        size_t read1 = std::fread(&addr, 1, 4, f);
        if (read1 == 0) break;
        if (read1 != 4) { err = "truncated fragment header"; std::fclose(f); return std::nullopt; }
        if (!read_exact(f, &size, 4)) { err = "truncated fragment header"; std::fclose(f); return std::nullopt; }
        InputImage::Frag fr; fr.addr = addr; fr.data.resize(size);
        if (!read_exact(f, fr.data.data(), size)) { err = "truncated fragment data"; std::fclose(f); return std::nullopt; }
        img.frags.push_back(std::move(fr));
    }

    std::fclose(f);
    return img;
}

bool write_output_file(const std::string& path, const OutputImage& out, std::string& err) {
    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) { err = "failed to open output file"; return false; }

    if (!write_exact(f, out.regs, 32u*4u)) { err = "failed to write header"; std::fclose(f); return false; }

    uint32_t addr = out.start_addr;
    uint32_t size = static_cast<uint32_t>(out.mem.size());
    if (!write_exact(f, &addr, 4) || !write_exact(f, &size, 4)) { err = "failed to write fragment header"; std::fclose(f); return false; }
    if (size && !write_exact(f, out.mem.data(), size)) { err = "failed to write fragment data"; std::fclose(f); return false; }

    std::fclose(f);
    return true;
}

#include <charconv>
bool parse_u32(const char* s, uint32_t& out) {
    if (!s || !*s) return false;
    int base = 10;
    if (s[0]=='0' && (s[1]=='x' || s[1]=='X')) { base=16; s+=2; }
    auto [ptr, ec] = std::from_chars(s, s + std::strlen(s), out, base);
    return ec == std::errc{} && ptr == s + std::strlen(s);
}
