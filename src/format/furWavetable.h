#ifndef _FUR_WAVETABLE_H
#define _FUR_WAVETABLE_H

#include "furCommon.h"

namespace Furnace {
  constexpr BlockMagic wavetableBlockMagic=MAGIC('W','A','V','E');
  constexpr LongMagic wavetableFileMgic={'-','F','u','r','n','a','c','e',' ','w','a','v','e','t','a','-'};
  constexpr u32 WAVE_MAX_WIDTH=256;
  constexpr u32 WAVE_MAX_HEIGHT=256;

  struct u32_2d {
    u32 x, y;
  };

  class Wavetable {
    string name;
    u32 width, height;
    vector<u32> data;
    public:
      Wavetable();
      Wavetable(const Wavetable& w);
      Wavetable(u32 w, u32 h, string n="");

      Wavetable& operator=(const Wavetable& w) {
        name=w.name;
        width=w.width;
        height=w.height;
        data=w.data;
        return *this;
      }

      bool operator==(const Wavetable& w);

      void setWidth(u32 w);
      void setHeight(u32 h);
      void setSize(u32 w, u32 h);
      void setName(string n);

      void setData(u32* wave, u32 n, bool setWidth=false);
      void setData(u32 index, u32 value);
      void setData(initList<u32> wave);

      u32 getWidth() const;
      u32 getHeight() const;
      u32_2d getSize() const;
      u32* getData();
      string getName() const;

      FileOperationError load(FILE* f);
      FileOperationError save(FILE* f);
  };

  class WavetableFile {
    Version version;
    Wavetable data;
    public:
      WavetableFile(Version ver, Wavetable wave);
      Version getVersion() const;
      Wavetable& getWavetable();

      FileOperationError load(FILE* f);
      FileOperationError save(FILE* f);
  };
};

#endif
