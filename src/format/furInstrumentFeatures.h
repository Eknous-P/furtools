#include "furCommon.h"

namespace Furnace {
  typedef unsigned short FeatureCode;
  #define FEATURE_CODE_DEF(x,y) (FeatureCode)x|(y<<8)

  constexpr FeatureCode InsFeatureCodeNA=FEATURE_CODE_DEF('N', 'A');
  constexpr FeatureCode InsFeatureCodeFM=FEATURE_CODE_DEF('F', 'M');
  constexpr FeatureCode InsFeatureCodeMA=FEATURE_CODE_DEF('M', 'A');
  constexpr FeatureCode InsFeatureCode64=FEATURE_CODE_DEF('6', '4');
  constexpr FeatureCode InsFeatureCodeGB=FEATURE_CODE_DEF('G', 'B');
  constexpr FeatureCode InsFeatureCodeSM=FEATURE_CODE_DEF('S', 'M');
  constexpr FeatureCode InsFeatureCodeO1=FEATURE_CODE_DEF('O', '1');
  constexpr FeatureCode InsFeatureCodeO2=FEATURE_CODE_DEF('O', '2');
  constexpr FeatureCode InsFeatureCodeO3=FEATURE_CODE_DEF('O', '3');
  constexpr FeatureCode InsFeatureCodeO4=FEATURE_CODE_DEF('O', '4');
  constexpr FeatureCode InsFeatureCodeLD=FEATURE_CODE_DEF('L', 'D');
  constexpr FeatureCode InsFeatureCodeSN=FEATURE_CODE_DEF('S', 'N');
  constexpr FeatureCode InsFeatureCodeN1=FEATURE_CODE_DEF('N', '1');
  constexpr FeatureCode InsFeatureCodeFD=FEATURE_CODE_DEF('F', 'D');
  constexpr FeatureCode InsFeatureCodeWS=FEATURE_CODE_DEF('W', 'S');
  constexpr FeatureCode InsFeatureCodeSL=FEATURE_CODE_DEF('S', 'L');
  constexpr FeatureCode InsFeatureCodeLS=FEATURE_CODE_DEF('L', 'S');
  constexpr FeatureCode InsFeatureCodeLW=FEATURE_CODE_DEF('L', 'W');
  constexpr FeatureCode InsFeatureCodeMP=FEATURE_CODE_DEF('M', 'P');
  constexpr FeatureCode InsFeatureCodeSU=FEATURE_CODE_DEF('S', 'U');
  constexpr FeatureCode InsFeatureCodeES=FEATURE_CODE_DEF('E', 'S');
  constexpr FeatureCode InsFeatureCodeX1=FEATURE_CODE_DEF('X', '1');
  constexpr FeatureCode InsFeatureCodeNE=FEATURE_CODE_DEF('N', 'E');
  constexpr FeatureCode InsFeatureCodeEF=FEATURE_CODE_DEF('E', 'F');
  constexpr FeatureCode InsFeatureCodePN=FEATURE_CODE_DEF('P', 'N');
  constexpr FeatureCode InsFeatureCodeS2=FEATURE_CODE_DEF('S', '2');
  constexpr FeatureCode InsFeatureCodeS3=FEATURE_CODE_DEF('S', '3');
  constexpr FeatureCode InsFeatureCodeEN=FEATURE_CODE_DEF('E', 'N');

  class InstrumentFeature {
    protected:
      FeatureCode code;
    public:
      InstrumentFeature();
      InstrumentFeature(FeatureCode c);
      InstrumentFeature(const InstrumentFeature& f);

      virtual FeatureCode getFeatureCode() const;
      virtual u32 getFeatureSize();

      virtual FileOperationError load(FILE* f, Version fileVersion);
      virtual FileOperationError save(FILE* f, Version fileVersion);
  };

  class InsFeatureName : public InstrumentFeature {
    public:
      string name;

      InsFeatureName();
      InsFeatureName(string n);
      InsFeatureName(const InsFeatureName& n);

      u32 getFeatureSize();

      FileOperationError load(FILE* f, Version fileVersion);
      FileOperationError save(FILE* f, Version fileVersion);
  };

  class InsFeatureFM : public InstrumentFeature {
    public:
      u8 opEnable;
      u8 opCount;

      u8 algorithm, feedback;
      u8 fms, fms2, ams, ams2;
      u8 opllPatchNum;
      u8 block;

      struct Operator {
        u8 level, mult;
        u8 attack, decay, sustain, decay2, release;
        u8 detune, detune2;
        u8 ksl, kvs;
        u8 dam, dvb, ssg;
        u8 ws;
        u8 ksr, sus, vib, am, egt;
      } fmOperator[4];

      InsFeatureFM();
      InsFeatureFM(const InsFeatureFM& f);

      void loadDefualtOPNInstrument();
      void loadDefualtOPLInstrument();
      void loadDefualtOPLLInstrument();

      u32 getFeatureSize();

      FileOperationError load(FILE* f, Version fileVersion);
      FileOperationError save(FILE* f, Version fileVersion);
  };

  enum InsMacroCodes : u8 {
    MacroVol=0,
    MacroArp=1,
    MacroDuty=2,
    MacroWave=3,
    MacroPitch=4,
    MacroEx1=5,
    MacroEx2=6,
    MacroEx3=7,
    MacroAlg=8,
    MacrpFb=9,
    MacroFms=10,
    MacroAms=11,
    MacroPanL=12,
    MacroPanR=13,
    MacroPhaseReset=14,
    MacroEx4=15,
    MacroEx5=16,
    MacroEx6=17,
    MacroEx7=18,
    MacroEx8=19,
    MacroEx9=20,
    MacroEx10=21,
    // there should not be an entry in macroEntries with this code
    MacroStop=255
  };

  enum InsMacroWordSize : u8 {
    MacroWord_U8=0,
    MacroWord_S8=1,
    MacroWord_S16=2,
    MacroWord_S32=3
  };

  enum InsMacroType : u8 {
    MacroTypeNormal,
    MacroTypeADSR,
    MacroTypeLFO
  };

  class InsFeatureMacro : public InstrumentFeature {
    public:
      struct Macro {
        InsMacroCodes code;
        u8 length;
        u8 loop, release;
        u8 mode;
        InsMacroWordSize wordSize;
        bool instantRelease;
        InsMacroType type;
        bool open;
        u8 delay;
        u8 speed;
        u32 data[255];
      };

      vector<Macro> macroEntries;

      InsFeatureMacro();
      InsFeatureMacro(const InsFeatureMacro& m);

      u32 getFeatureSize();

      FileOperationError load(FILE* f, Version fileVersion);
      FileOperationError save(FILE* f, Version fileVersion);
  };

}