#ifndef PNM_IO_H
#define PNM_IO_H

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <fstream>
#include <sstream>

struct Image {
    int width;
    int height;
    int channels; // 1 for P5, 3 for P6
    std::vector<unsigned char> data; // size = width*height*channels
    std::string magic; // "P5" or "P6"
};

inline std::string read_token(std::ifstream &ifs) {
    std::string token;
    // skip whitespace
    char c;
    while (ifs.get(c)) {
        if (!isspace((unsigned char)c)) {
            ifs.unget();
            break;
        }
    }
    if (!(ifs >> token)) throw std::runtime_error("Failed to read token");
    return token;
}

inline Image read_pnm(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) throw std::runtime_error("Cannot open input file");

    std::string magic;
    if (!(ifs >> magic)) throw std::runtime_error("Invalid PNM file: missing magic");
    if (magic != "P5" && magic != "P6") throw std::runtime_error("Unsupported PNM format (only P5/P6)");

    int width=0, height=0, maxv=0;
    ifs >> width >> height >> maxv;
    if (!ifs) throw std::runtime_error("Invalid PNM header");
    if (maxv != 255) throw std::runtime_error("Unsupported max value (must be 255)");

    ifs.get(); // consume newline

    Image img;
    img.width = width;
    img.height = height;
    img.channels = (magic == "P5") ? 1 : 3;
    img.magic = magic;
    size_t sz = static_cast<size_t>(width) * static_cast<size_t>(height) * img.channels;
    img.data.resize(sz);
    ifs.read(reinterpret_cast<char*>(img.data.data()), sz);
    if (static_cast<size_t>(ifs.gcount()) != sz) throw std::runtime_error("Unexpected EOF while reading image data");
    return img;
}

inline void write_pnm(const std::string &filename, const Image &img) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) throw std::runtime_error("Cannot open output file");
    ofs << img.magic << "\n" << img.width << " " << img.height << "\n255\n";
    ofs.write(reinterpret_cast<const char*>(img.data.data()), img.data.size());
    if (!ofs) throw std::runtime_error("Failed to write output file");
}

#endif // PNM_IO_H