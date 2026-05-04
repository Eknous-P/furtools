#include "furInstrument.h"

namespace Furnace {
  Instrument::Instrument():
    version(0),
    type(InsSTD),
    features({}) {}
  
  Instrument::Instrument(Version v, InsTypes t):
    version(v),
    type(t),
    features({}) {}
  
  Instrument::Instrument(const Instrument& ins) {
    common_assert(false, "not implemented! sorry...");
  }

  InstrumentFeature* Instrument::newFeature(FeatureCode c) {
    switch (c) {
      case InsFeatureCodeNA:
        return new InsFeatureName(version);
      // case InsFeatureCodeFM:
      //   return new InsFeatureFM(version);
      case InsFeatureCodeMA:
        return new InsFeatureMacro(version);
      default:
        common_assert(true, "invalid feature code!");
        return NULL;
    }
  }

  void Instrument::addFeature(FeatureCode c) {
    InstrumentFeature* feature = newFeature(c);
    if (feature==NULL) return;
    features.push_back(feature);
  }

  void Instrument::addFeature(InstrumentFeature* f) {
    if (f==NULL) return;
    features.push_back(f);
  }

  FileOperationError Instrument::loadInternal(FILE* f) {
    version = fReadInt<Version>(f);
    type = (InsTypes)fReadInt<u16>(f);

    for (size_t i=0; i<features.size(); i++)
      delete features[i];
    features.clear();
    FeatureCode featureCode;
    InstrumentFeature* feature;
    do {
      fread(&featureCode, 1, 2, f);
      if (feof(f)) return fileEOF;
      if (featureCode==InsFeatureCodeEN) break;
      feature = newFeature(featureCode);
      if (feature==NULL) return fileInvalidInsFeature; 
      feature->load(f);
      addFeature(feature);
    } while (1);
    return fileOK;
  }

  FileOperationError Instrument::loadFile(FILE* f) {
    BlockMagic magic;
    fread(&magic, 1, 4, f);
    if (magic!=InsFileMagic) return fileMagicError;
    return loadInternal(f);
  }

  FileOperationError Instrument::loadBlock(FILE* f) {
    BlockMagic magic;
    fread(&magic, 1, 4, f);
    if (magic!=InsBlockMagic) return fileMagicError;
    u32 blockSize = fReadInt<u32>(f);
    u32 curSeek=ftell(f);
    FileOperationError err = loadInternal(f);
    if (err==fileOK) {
      if (ftell(f)-curSeek != blockSize) {
        return fileBlockSizeMismatch;
      }
      return fileOK;
    }
    return err;
  }

  FileOperationError Instrument::saveInternal(FILE* f) {
    fWriteInt<Version>(version, f);
    fWriteInt<InsTypes>(type, f);
    InsFeatureWaveList* waveList=NULL;
    for (InstrumentFeature*& feature : features) {
      fWriteInt<FeatureCode>(feature->getFeatureCode(), f);
      fWriteInt<u16>(feature->getFeatureSize(), f);
      FileOperationError err = feature->save(f);
      if (feature->getFeatureCode() == InsFeatureCodeLW || feature->getFeatureCode() == InsFeatureCodeWL) {
        waveList=(InsFeatureWaveList*)feature;
      }
      if (err != fileOK) return err;
    }
    if (waveList) {
      fWriteInt<FeatureCode>(InsFeatureCodeEN, f);
      vector<u32> waveIndexes;
      for (Wavetable& w:waveList->waves) {
        waveIndexes.push_back(ftell(f));
        w.save(f);
      }
      waveList->saveContinue(f, waveIndexes);
    }
    return fileOK;
  }

  FileOperationError Instrument::saveFile(FILE* f) {
    fwrite(&InsFileMagic, 1, 4, f);
    return saveInternal(f);
  }

  FileOperationError Instrument::saveBlock(FILE* f) {
    fwrite(&InsBlockMagic, 1, 4, f);
    u32 sizeSeek = ftell(f);
    fWriteInt<u32>(0, f);
    FileOperationError err = saveInternal(f);
    if (err != fileOK) return err;
    u32 lastSeek = ftell(f);
    u32 blockSize = lastSeek-sizeSeek-4;
    fseek(f, sizeSeek, SEEK_SET);
    fWriteInt<u32>(blockSize, f);
    fseek(f, lastSeek, SEEK_SET);
    return fileOK;
  }
}
