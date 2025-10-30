#pragma once
#include <cstdint>
#include <cstddef>

// === Вариант 2  ===
inline constexpr std::size_t MEMORY_SIZE      = 0x80000;
inline constexpr unsigned     ADDRESS_LEN     = 19;

inline constexpr std::size_t  CACHE_SIZE      = 2048;
inline constexpr std::size_t  CACHE_LINE_SIZE = 32;
inline constexpr std::size_t  CACHE_LINE_COUNT= 64;
inline constexpr std::size_t  CACHE_SET_COUNT = 16;
inline constexpr std::size_t  CACHE_WAY       = 4;

inline constexpr unsigned     CACHE_OFFSET_LEN= 5;  // log2(32)
inline constexpr unsigned     CACHE_INDEX_LEN = 4;  // log2(16)
inline constexpr unsigned     CACHE_TAG_LEN   = 10; // 19 - 5 - 4

static_assert((1u << ADDRESS_LEN) == MEMORY_SIZE, "ADDRESS_LEN must match MEMORY_SIZE");
static_assert(CACHE_SIZE == CACHE_LINE_SIZE * CACHE_LINE_COUNT, "CACHE_SIZE mismatch");
static_assert(CACHE_LINE_COUNT == CACHE_SET_COUNT * CACHE_WAY,   "sets*ways != lines");
static_assert((1u << CACHE_OFFSET_LEN) == CACHE_LINE_SIZE,       "offset bits mismatch");
static_assert((1u << CACHE_INDEX_LEN)  == CACHE_SET_COUNT,       "index bits mismatch");
static_assert((CACHE_TAG_LEN + CACHE_INDEX_LEN + CACHE_OFFSET_LEN) == ADDRESS_LEN,
              "tag+index+offset must equal ADDRESS_LEN");

// Конфигурация кэша по заданию:
// look-through + write-back + write-allocate
enum class WritePolicy   { WriteBack };
enum class AllocatePolicy{ WriteAllocate };
enum class Topology      { LookThrough };
