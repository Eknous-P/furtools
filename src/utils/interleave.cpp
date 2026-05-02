#include "interleave.h"

namespace Utils {
  int fileInterleave(FILE *in1, FILE *in2, FILE *out) {
    size_t size = getFileSize(in1);
    if (size != getFileSize(in2)) return 1;
    rewind(in1); rewind(in2);
    rewind(out);
    for (size_t i=0; i<size; i++) {
      fputc(fgetc(in1), out);
      fputc(fgetc(in2), out);
    }
    return 0;
  }
}
