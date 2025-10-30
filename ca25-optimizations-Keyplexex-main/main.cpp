#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <omp.h>

#include "pnm_io.h"

// Macros for build-time defaults
#ifndef STR2
#define STR2(x) #x
#define STR(x) STR2(x)
#endif

#ifdef SCHEDULE_KIND
const char* BUILD_SCHEDULE = STR(SCHEDULE_KIND);
#else
const char* BUILD_SCHEDULE = "static";
#endif

#ifndef SCHEDULE_CHUNK_SIZE
#define SCHEDULE_CHUNK_SIZE 0
#endif
const int BUILD_CHUNK = SCHEDULE_CHUNK_SIZE;

void print_help(const char* prog) {
    std::cout << "Usage: " << prog << " --input <in.pnm> --output <out.pnm> "
              << "[--coef <float in [0.0,0.5))] [--schedule <static|dynamic>] [--chunk_size <0 or more>] "
              << "[--no-omp | --omp-threads <num|default>]\n";
    std::cout << "Example: " << prog << " --input in.pnm --output out.pnm --coef 0.00390625 --schedule static --chunk_size 0 --omp-threads default\n";
}

bool parse_double(const char* s, double &out) {
    char* end;
    out = strtod(s, &end);
    return end && *end == '\0';
}

bool parse_int(const char* s, int &out) {
    char* end;
    long v = strtol(s, &end, 10);
    if (end && *end == '\0') { out = static_cast<int>(v); return true; }
    return false;
}

int main(int argc, char** argv) {
    std::string input_file, output_file;
    double coef = 0.0;
    bool coef_set = false;
    std::string schedule_str = BUILD_SCHEDULE;
    int chunk_size = BUILD_CHUNK;
    bool no_omp = false;
    bool omp_threads_set = false;
    int omp_threads_value = 0;
    bool omp_threads_default = false;

    if (argc == 1) {
        print_help(argv[0]);
        return 1;
    }

    for (int i=1; i<argc; i++) {
        std::string a = argv[i];
        if (a == "--help") { print_help(argv[0]); return 0; }
        else if (a == "--input") {
            if (++i >= argc) { std::fprintf(stderr, "Missing value for --input\n"); return 2; }
            input_file = argv[i];
        } else if (a == "--output") {
            if (++i >= argc) { std::fprintf(stderr, "Missing value for --output\n"); return 2; }
            output_file = argv[i];
        } else if (a == "--coef") {
            if (++i >= argc) { std::fprintf(stderr, "Missing value for --coef\n"); return 2; }
            if (!parse_double(argv[i], coef)) { std::fprintf(stderr, "Invalid coef value\n"); return 2; }
            coef_set = true;
        } else if (a == "--schedule") {
            if (++i >= argc) { std::fprintf(stderr, "Missing value for --schedule\n"); return 2; }
            schedule_str = argv[i];
            if (schedule_str != "static" && schedule_str != "dynamic") { std::fprintf(stderr, "Invalid schedule\n"); return 2; }
        } else if (a == "--chunk_size") {
            if (++i >= argc) { std::fprintf(stderr, "Missing value for --chunk_size\n"); return 2; }
            if (!parse_int(argv[i], chunk_size) || chunk_size < 0) { std::fprintf(stderr, "Invalid chunk_size\n"); return 2; }
        } else if (a == "--no-omp") {
            no_omp = true;
        } else if (a == "--omp-threads") {
            if (++i >= argc) { std::fprintf(stderr, "Missing value for --omp-threads\n"); return 2; }
            std::string v = argv[i];
            if (v == "default") { omp_threads_default = true; omp_threads_set = true; }
            else {
                if (!parse_int(v.c_str(), omp_threads_value) || omp_threads_value <= 0) { std::fprintf(stderr, "Invalid omp-threads\n"); return 2; }
                omp_threads_set = true;
            }
        } else {
            std::fprintf(stderr, "Unknown argument: %s\n", a.c_str());
            return 2;
        }
    }

    if (input_file.empty() || output_file.empty()) { std::fprintf(stderr, "--input and --output required\n"); return 2; }
    if (!coef_set) coef = 0.0;
    if (coef < 0.0 || coef >= 0.5) { std::fprintf(stderr, "--coef must be in [0.0, 0.5)\n"); return 2; }
    if (no_omp && omp_threads_set) { std::fprintf(stderr, "Cannot use --no-omp and --omp-threads together\n"); return 2; }

    try {
        Image img = read_pnm(input_file);
        const int W = img.width;
        const int H = img.height;
        const int C = img.channels;
        const size_t N = static_cast<size_t>(W) * H;
        Image out = img;
        std::vector<unsigned char> &in_data = img.data;
        std::vector<unsigned char> &out_data = out.data;

        if (no_omp) omp_set_num_threads(1);
        else if (omp_threads_set && !omp_threads_default) omp_set_num_threads(omp_threads_value);

        omp_sched_t sched_kind = (schedule_str == "dynamic") ? omp_sched_dynamic : omp_sched_static;
        omp_set_schedule(sched_kind, chunk_size);

        bool is_constant = true;
        unsigned char first = in_data[0];
        for (auto v : in_data) {
            if (v != first) { is_constant = false; break; }
        }

        double t0 = omp_get_wtime();
        int threads_used = 1;

        if (!is_constant) {
            size_t ignore_count = static_cast<size_t>(std::llround(coef * N));

            int max_threads = omp_get_max_threads();
            std::vector<std::vector<int>> thread_hists(max_threads, std::vector<int>(C * 256, 0));
            std::vector<int> merged_hist(C * 256, 0);

            #pragma omp parallel
            {
                int tid = omp_get_thread_num();
                #pragma omp single
                threads_used = omp_get_num_threads();
                auto &local = thread_hists[tid];
                #pragma omp for schedule(runtime)
                for (size_t p = 0; p < N; ++p) {
                    size_t base = p * C;
                    for (int ch = 0; ch < C; ++ch) {
                        unsigned char v = in_data[base + ch];
                        local[ch * 256 + v] += 1;
                    }
                }
            }

            for (int t = 0; t < max_threads; ++t) {
                for (int i = 0; i < C * 256; ++i) merged_hist[i] += thread_hists[t][i];
            }

            std::vector<int> minVal(C, 0), maxVal(C, 255);
            for (int ch = 0; ch < C; ++ch) {
                size_t cum = 0;
                for (int v = 0; v < 256; ++v) {
                    cum += merged_hist[ch * 256 + v];
                    if (cum > ignore_count) { minVal[ch] = v; break; }
                }
                cum = 0;
                for (int v = 255; v >= 0; --v) {
                    cum += merged_hist[ch * 256 + v];
                    if (cum > ignore_count) { maxVal[ch] = v; break; }
                }
            }

            int global_min = *std::min_element(minVal.begin(), minVal.end());
            int global_max = *std::max_element(maxVal.begin(), maxVal.end());
            double scale = (global_max != global_min) ? 255.0 / static_cast<double>(global_max - global_min) : 1.0;

            #pragma omp parallel
            {
                #pragma omp for schedule(runtime)
                for (size_t p = 0; p < N; ++p) {
                    size_t base = p * C;
                    for (int ch = 0; ch < C; ++ch) {
                        int v = static_cast<int>(in_data[base + ch]);
                        int y = static_cast<int>(std::round(static_cast<double>(v - global_min) * scale));
                        if (y < 0) y = 0;
                        if (y > 255) y = 255;
                        out_data[base + ch] = static_cast<unsigned char>(y);
                    }
                }
            }
        }

        double t1 = omp_get_wtime();
        double elapsed_ms = (t1 - t0) * 1000.0;
        printf("Time (%i threads): %lg\n", threads_used, elapsed_ms);

        write_pnm(output_file, out);

    } catch (const std::exception &ex) {
        std::fprintf(stderr, "Error: %s\n", ex.what());
        return 3;
    }

    return 0;
}