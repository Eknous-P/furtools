#include "furWavetable.h"
#include "common.h"
#include "furCommon.h"
#include <cstdio>
#include <cstring>

namespace Furnace {
  Wavetable::Wavetable():
    name(""),
    width(0), height(0) {
      data.resize(WAVE_MAX_WIDTH);
    }
  
  Wavetable::Wavetable(const Wavetable& w) {
    name = w.name;
    width = w.width;
    height = w.height;
    data = w.data;
  }

  Wavetable::Wavetable(u32 w, u32 h, string n) {
    common_assert(w<=WAVE_MAX_WIDTH, "invalid wavetable width!");
    common_assert(h<=WAVE_MAX_HEIGHT, "invalid wavetable height!");
    name = n;
    width = w;
    height = h;
    data.resize(WAVE_MAX_WIDTH);
  }

  bool Wavetable::operator==(const Wavetable& w) {
    if (name!=w.name || width!=w.width || height!=w.height) return false;
    if (memcmp(&data[0], &w.data[0], sizeof(u32)*width)) return false;
    return true;
  }

  void Wavetable::setWidth(u32 w) {width=w;}
  void Wavetable::setHeight(u32 h) {height=h;}
  void Wavetable::setSize(u32 w, u32 h) {width=w; height=h;}

  void Wavetable::setData(u32* wave, u32 n, bool setWidth) {
    common_assert(n<=WAVE_MAX_WIDTH, "invalid wavetable size!");
    if (setWidth) width=n;
    for (u32 i=0; i<n; i++) {
      data[i] = wave[i];
    }
  }

  void Wavetable::setData(u32 index, u32 value) {
    common_assert(index<WAVE_MAX_WIDTH, "invalid wavetable index!");
    data[index]=value;
  }

  void Wavetable::setData(initList<u32> wave) {
    common_assert(wave.size()<=WAVE_MAX_WIDTH, "invalid wavetable array size!");
    data=wave;
  }

  u32 Wavetable::getWidth() const {
    return width;
  }

  u32 Wavetable::getHeight() const {
    return height;
  }

  u32_2d Wavetable::getSize() const {
    return {width, height};
  }

  u32* Wavetable::getData() {
    return &data[0];
  }

  string Wavetable::getName() const {
    return name;
  }

  FileOperationError Wavetable::load(FILE* f) {
    BlockMagic magic;
    fread(&magic, 1, 4, f);
    if (memcmp(&magic, &wavetableBlockMagic, 4)!=0) return fileMagicError;
    fReadInt<u32>(f);
    char c;
    name="";
    do {
      c=fgetc(f);
      name+=c;
    } while (c!='\0');
    width=fReadInt<u32>(f);
    fReadInt<u32>(f); // reserved
    height=fReadInt<u32>(f);
    return fileOK;
  }

  FileOperationError Wavetable::save(FILE* f) {
    fwrite(&wavetableBlockMagic, 4, 1, f);
    u32 blockSize = 
      name.size() + 1 /*null terminator*/ +
      4 + 4 + 4 +
      data.size() * sizeof(u32);
    fWriteInt<u32>(blockSize, f);
    fprintf(f, "%s", name.c_str());
    fWriteInt<char>(0, f);
    fWriteInt<u32>(width, f);
    fWriteInt<u32>(0, f); // reserved
    fWriteInt<u32>(height, f);
    for (u32& v:data) {
      fWriteInt<u32>(v,f);
    }
    return fileOK;
  }

  WavetableFile::WavetableFile(Version ver, Wavetable wave):
    version(ver),
    data(wave) {}
  
  Version WavetableFile::getVersion() const {
    return version;
  }

  Wavetable& WavetableFile::getWavetable() {
    return data;
  }

  FileOperationError WavetableFile::load(FILE* f) {
    LongMagic magic;
    fread(&magic, 1, 16, f);
    if (memcmp(&magic, &wavetableFileMgic, 16)!=0) return fileMagicError;
    version=fReadInt<Version>(f);
    fReadInt<Version>(f);
    return data.load(f);
  }

  FileOperationError WavetableFile::save(FILE* f) {
    fwrite(&wavetableFileMgic, 1, 16, f);;
    fWriteInt<Version>(version, f);
    fWriteInt<u16>(0, f);
    return data.save(f);
  }
}
