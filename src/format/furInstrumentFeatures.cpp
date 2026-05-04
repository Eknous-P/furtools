#include "furInstrumentFeatures.h"
#include <functional>

namespace Furnace {
  // InstrumentFeature

  InstrumentFeature::InstrumentFeature():
    fileVersion(0),
    code(0) {}

  InstrumentFeature::InstrumentFeature(Version v, FeatureCode c):
    fileVersion(v),
    code(c) {}

  InstrumentFeature::InstrumentFeature(const InstrumentFeature& f):
    fileVersion(f.fileVersion),
    code(f.code) {}

  Version InstrumentFeature::getFeatureVersion() const {
    return fileVersion;
  }

  FeatureCode InstrumentFeature::getFeatureCode() const {
    return code;
  }

  u32 InstrumentFeature::getFeatureSize() {
    return 0;
  }

  FileOperationError InstrumentFeature::load(FILE* f) {
    return fileOK;
  }

  FileOperationError InstrumentFeature::save(FILE* f) {
    return fileOK;
  }

  // InsFeatureName

  InsFeatureName::InsFeatureName(Version v):
    InstrumentFeature(v, InsFeatureCodeNA),
    name("") {}
  
  InsFeatureName::InsFeatureName(Version v, string n):
    InstrumentFeature(v, InsFeatureCodeNA),
    name(n) {}
  
  InsFeatureName::InsFeatureName(const InsFeatureName& n):
    InstrumentFeature(n.fileVersion, InsFeatureCodeNA),
    name(n.name) {}
  
  u32 InsFeatureName::getFeatureSize() {
    return name.size()+1;
  }
  
  FileOperationError InsFeatureName::load(FILE* f) {
    int c;
    name.clear();
    while ((c=fgetc(f))) {
      if (c==EOF) return fileEOF;
      name+=(char)c;
    }
    return fileOK;
  }

  FileOperationError InsFeatureName::save(FILE* f) {
    fprintf(f, "%s", name.c_str());
    fWriteInt<char>(0, f);
    return fileOK;
  }

  // InsFeatureMacro
  InsFeatureMacro::InsFeatureMacro(Version v):
    InstrumentFeature(v, InsFeatureCodeMA),
    macroEntries({}) {}
  
  InsFeatureMacro::InsFeatureMacro(const InsFeatureMacro& m):
    InstrumentFeature(m.fileVersion, InsFeatureCodeMA),
    macroEntries(m.macroEntries) {}
  
  u32 InsFeatureMacro::getFeatureSize() {
    u32 ret = 2; // header size value
    for (auto& i: macroEntries) {
      ret += 8;
      switch (i.wordSize) {
        case MacroWord_S8:
        case MacroWord_U8:
          ret += i.length;
          break;
        case MacroWord_S16:
          ret += i.length*2;
          break;
        case MacroWord_S32:
          ret += i.length*4;
          break;
        default:
          common_assert(false, "invalid macro word size!");
      }
    }
    return ret + 1; // ff terminator
  }

  FileOperationError InsFeatureMacro::load(FILE* f) {
    macroEntries.clear();
    if (fReadInt<u16>(f) != 8) return fileInvalidHeaderSize;
    InsMacroCodes code;
    while (1) {
      code = fReadInt<InsMacroCodes>(f);
      if (code==MacroStop) break;
      if (feof(f)) return fileEOF;

      Macro curMacro;
      curMacro.code = code;
      curMacro.length = fReadInt<u8>(f);
      curMacro.loop = fReadInt<u8>(f);
      curMacro.release = fReadInt<u8>(f);
      curMacro.mode = fReadInt<u8>(f);

      u8 macroFlags = fReadInt<u8>(f);
      curMacro.wordSize = InsMacroWordSize(macroFlags>>6);
      curMacro.instantRelease = macroFlags&(1<<3);
      curMacro.type = InsMacroType((macroFlags>>1)&3);
      curMacro.open = macroFlags&1;

      curMacro.delay = fReadInt<u8>(f);
      curMacro.speed = fReadInt<u8>(f);

      std::function<void(FILE*,int)> macroDataRead;
      switch (curMacro.wordSize) {
        case MacroWord_U8:
          macroDataRead=[&curMacro](FILE* f, int i){curMacro.data[i]=fReadInt<u8>(f);};
          break;
        case MacroWord_S8:
          macroDataRead=[&curMacro](FILE* f, int i){curMacro.data[i]=fReadInt<s8>(f);};
          break;
        case MacroWord_S16:
          macroDataRead=[&curMacro](FILE* f, int i){curMacro.data[i]=fReadInt<s16>(f);};
          break;
        case MacroWord_S32:
          macroDataRead=[&curMacro](FILE* f, int i){curMacro.data[i]=fReadInt<s32>(f);};
          break;
      }
      for (int i=0; i<curMacro.length; i++) {
        macroDataRead(f, i);
      }
      macroEntries.push_back(curMacro);
    }
    return fileOK;
  }

  FileOperationError InsFeatureMacro::save(FILE* f) {
    if (macroEntries.empty()) return fileEmptyMacro;
    fWriteInt<u16>(8, f);
    for (Macro& m:macroEntries) {
      fWriteInt<InsMacroCodes>(m.code, f);
      fWriteInt<u8>(m.length, f);
      fWriteInt<u8>(m.loop, f);
      fWriteInt<u8>(m.release, f);
      fWriteInt<u8>(m.mode, f);
      u8 macroFlags=0;
      if (m.open) macroFlags |= (1<<0);
      macroFlags |= (m.type&3)<<1;
      if (m.instantRelease) macroFlags |= (1<<3);
      macroFlags |= (m.wordSize&3)<<6;
      fWriteInt<u8>(macroFlags, f);
      fWriteInt<u8>(m.delay, f);
      fWriteInt<u8>(m.speed, f);

      std::function<void(FILE*,int)> macroDataWrite;
      switch (m.wordSize) {
        case MacroWord_U8:
          macroDataWrite=[&m](FILE* f, int i){fWriteInt<u8>((u8)m.data[i],f);};
          break;
        case MacroWord_S8:
          macroDataWrite=[&m](FILE* f, int i){fWriteInt<s8>((s8)m.data[i],f);};
          break;
        case MacroWord_S16:
          macroDataWrite=[&m](FILE* f, int i){fWriteInt<s16>((s16)m.data[i],f);};
          break;
        case MacroWord_S32:
          macroDataWrite=[&m](FILE* f, int i){fWriteInt<s32>((s32)m.data[i],f);};
          break;
      }
      for (int i=0; i<m.length; i++) {
        macroDataWrite(f,i);
      }
    }
    fWriteInt<InsMacroCodes>(MacroStop, f);
    return fileOK;
  }

  InsFeatureWaveList::InsFeatureWaveList(Version v):
    InstrumentFeature(v, v>=233?InsFeatureCodeLW:InsFeatureCodeWL),
    waves({}) {}
  
  InsFeatureWaveList::InsFeatureWaveList(const InsFeatureWaveList& w):
    InstrumentFeature(w.fileVersion, w.getFeatureCode()),
    waves(w.waves) {}

  u32 InsFeatureWaveList::getFeatureSize() {
    if (fileVersion>=233) {
      return 2 + 6 * waves.size();
    } else {
      return 1 + 5 * waves.size();
    }
  }

  FileOperationError InsFeatureWaveList::load(FILE* f) {
    waves.clear();
    FileOperationError waveLoadErr;
    if (fileVersion>=233) {
      u16 wavesNum = fReadInt<u16>(f);
      waveIndexes.resize(wavesNum);
      waves.resize(wavesNum);
      for (u16 i=0; i<wavesNum; i++) {
        u16 waveIndex = fReadInt<u16>(f);
        waveIndexes[i] = waveIndex;
      }
      for (u16 i=0; i<wavesNum; i++) {
        u32 wavePointer = fReadInt<u32>(f);
        
        size_t curSeek = ftell(f);
        fseek(f, wavePointer, SEEK_SET);
        waveLoadErr = waves[i].load(f);
        if (waveLoadErr!= fileOK) goto fail;
        fseek(f, curSeek, SEEK_SET);
      }
    } else {
      u16 wavesNum = fReadInt<u8>(f);
      waves.resize(wavesNum);
      for (u16 i=0; i<wavesNum; i++) {
        u8 waveIndex = fReadInt<u8>(f);
        waveIndexes[i] = waveIndex;
      }
      for (u16 i=0; i<wavesNum; i++) {
        u32 wavePointer = fReadInt<u32>(f);
        
        size_t curSeek = ftell(f);
        fseek(f, wavePointer, SEEK_SET);
        waveLoadErr = waves[i].load(f);
        if (waveLoadErr!= fileOK) goto fail;
        fseek(f, curSeek, SEEK_SET);
      }
    }
    return fileOK;
    fail:
    waveIndexes.clear();
    waves.clear();
    return waveLoadErr;
  }

  FileOperationError InsFeatureWaveList::save(FILE* f) {
    if (fileVersion>=233) {
      u16 wavesNum = waves.size();
      if (wavesNum > 32768) return fileTooManyItems;
      fWriteInt<u16>(wavesNum,f);
      for (u16& i:waveIndexes) {
        fWriteInt<u16>(i, f);
      }
      wavePtrsPtr = ftell(f);
      for (u16 i=0; i<wavesNum; i++) {
        fWriteInt<u32>(0, f);
      }
    } else {
      u16 wavesNum = waves.size();
      if (wavesNum > 256) return fileTooManyItems;
      fWriteInt<u8>(wavesNum,f);
      for (u16& i:waveIndexes) {
        fWriteInt<u8>(i, f);
      }
      for (u16 i=0; i<wavesNum; i++) {
        fWriteInt<u32>(0, f);
      }
      wavePtrsPtr = ftell(f);
    }
    return fileOK;
  }

  FileOperationError InsFeatureWaveList::saveContinue(FILE* f, vector<u32> ptrs) {
    size_t curSeek = ftell(f);
    fseek(f, wavePtrsPtr, SEEK_SET);
    for (u32& i:ptrs) {
      fWriteInt<u32>(i, f);
    }
    fseek(f, curSeek, SEEK_SET);
    return fileOK;
  }
}
