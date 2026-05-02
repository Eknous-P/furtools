#include "common.h"

size_t getFileSize(FILE *f) {
  size_t lastSeek = ftell(f);
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, lastSeek, SEEK_SET);
  return size;
}
