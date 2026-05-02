#ifndef _COMMON_H
#define _COMMON_H

#include <cstdio>
#include <cstring>
#include <cassert>
#include <vector>
#include <string>
#include <initializer_list>

#define FAIL_ON_ASSERT
#define PRINT_ASSERT_FAILS

typedef unsigned  char u8;
typedef   signed  char s8;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned   int u32;
typedef   signed   int s32;
typedef          float f32;

using std::vector;
using std::string;
using std::initializer_list;
#define initList initializer_list

#ifdef FAIL_ON_ASSERT
#define common_assert(cond, msg) assert(cond && __FILE__ ": " msg)
#else
#ifdef PRINT_ASSERT_FAILS
#define common_assert(cond, msg) if(!(cond)) printf(__FILE__ ": " msg); return -1;
#else
#define common_assert(cond, msg)
#endif
#endif

template<typename T> size_t fWriteInt(T a, FILE* f) {
  size_t r = fwrite(&a, sizeof(T), 1, f);
  common_assert(!feof(f), "unexpected EOF!");
  return r;
}

template<typename T> T fReadInt(FILE* f) {
  T v;
  fread(&v, sizeof(T), 1, f);
  common_assert(!feof(f), "unexpected EOF!");
  return v;
}

size_t getFileSize(FILE* f);

#endif
