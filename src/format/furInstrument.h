#include "furCommon.h"
#include "furInstrumentFeatures.h"

namespace Furnace {
  enum InsTypes : u16 {
    InsSTD=0,
    InsOPN=1,
    InsGB=2,
    InsC64=3,
    InsSample=4,
    InsPCE=5,
    InsAY10=6,
    InsAY30=7,
    InsTIA=8,
    InsSAA=9,
    InsVIC=10,
    InsPET=11,
    InsVRC6=12,
    InsOPLL=13,
    InsOPL=14,
    InsFDS=15,
    InsVB=16,
    Ins163=17,
    InsSCC=18,
    InsOPZ=19,
    InsPOK=20,
    InsPCS=21,
    InsSwan=22,
    InsLynx=23,
    InsVERA=24,
    InsX1=25,
    InsV6Saw=26,
    Ins5506=27,
    InsMPCM=28,
    InsSNES=29,
    InsSU=30,
    InsWSG=31,
    InsOPLD=32,
    InsOPM=33,
    InsNES=34,
    Ins6258=35,
    Ins6295=36,
    InsAPCMA=37,
    InsAPCMB=38,
    InsSPCM=39,
    InsQSnd=40,
    InsYMZ=41,
    InsRF5C=42,
    Ins5232=43,
    InsT6W=44,
    Ins7232=45,
    InsGA20=46,
    InsPMini=47,
    Ins8521=48,
    InsPV1K=49,
    InsK05=50,
    InsTED=52,
    InsC140=53,
    InsC219=54,
    InsESFM=55,
    InsPNN=56,
    InsPNS=57,
    InsDave=58,
    InsNDS=59,
    InsGBAD=60,
    InsGBAM=61,
    InsBif=62,
    InsSID2=63,
    InsSVis=64,
    InsuPD=65,
    InsSID3=66
  };

  constexpr BlockMagic InsFileMagic=MAGIC('F','I','N','S');
  constexpr BlockMagic InsBlockMagic=MAGIC('I','N','S','2');
  
  class Instrument {
    private:
      Version version;
      InsTypes type;
      vector<InstrumentFeature*> features;

      InstrumentFeature* newFeature(FeatureCode c);

      FileOperationError loadInternal(FILE* f);
      FileOperationError saveInternal(FILE* f);
    public:
      Instrument();
      Instrument(Version v, InsTypes t);
      Instrument(const Instrument& ins);

      void addFeature(FeatureCode c);
      void addFeature(InstrumentFeature* f);

      FileOperationError loadFile(FILE* f);
      FileOperationError saveFile(FILE* f);
      FileOperationError loadBlock(FILE* f);
      FileOperationError saveBlock(FILE* f);
  };
}
