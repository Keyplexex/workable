#pragma once
#include <cstdio>
#include <string>

inline int fail(int code, const std::string& msg) {
    std::fprintf(stderr, "error: %s\n", msg.c_str());
    return code;
}
