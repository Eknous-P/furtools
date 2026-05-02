#include "furInstrumentFeatures.h"
#include "furCommon.h"
#include <functional>

namespace Furnace {
  // InstrumentFeature

  InstrumentFeature::InstrumentFeature():
    code(0) {}

  InstrumentFeature::InstrumentFeature(FeatureCode c):
    code(c) {}

  InstrumentFeature::InstrumentFeature(const InstrumentFeature& f):
    code(f.code) {}

  FeatureCode InstrumentFeature::getFeatureCode() const {
    return code;
  }

  u32 InstrumentFeature::getFeatureSize() {
    return 0;
  }

  FileOperationError InstrumentFeature::load(FILE* f, Version fileVersion) {
    return fileOK;
  }

  FileOperationError InstrumentFeature::save(FILE* f, Version fileVersion) {
    return fileOK;
  }

  // InsFeatureName

  InsFeatureName::InsFeatureName():
    InstrumentFeature(InsFeatureCodeNA),
    name("") {}
  
  InsFeatureName::InsFeatureName(string n):
    InstrumentFeature(InsFeatureCodeNA),
    name(n) {}
  
  InsFeatureName::InsFeatureName(const InsFeatureName& n):
    InstrumentFeature(InsFeatureCodeNA),
    name(n.name) {}
  
  u32 InsFeatureName::getFeatureSize() {
    return name.size()+1;
  }
  
  FileOperationError InsFeatureName::load(FILE* f, Version fileVersion) {
    int c;
    name.clear();
    while ((c=fgetc(f))) {
      if (c==EOF) return fileEOF;
      name+=(char)c;
    }
    return fileOK;
  }

  FileOperationError InsFeatureName::save(FILE* f, Version fileVersion) {
    fprintf(f, "%s", name.c_str());
    fWriteInt<char>(0, f);
    return fileOK;
  }

  // InsFeatureMacro
  InsFeatureMacro::InsFeatureMacro():
    InstrumentFeature(InsFeatureCodeMA),
    macroEntries({}) {}
  
  InsFeatureMacro::InsFeatureMacro(const InsFeatureMacro& m):
    InstrumentFeature(InsFeatureCodeMA),
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
    return ret;
  }

  FileOperationError InsFeatureMacro::load(FILE* f, Version fileVersion) {
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
}