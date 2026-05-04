#include "tool.h"
#include <QAudioDecoder>
#include "furnace.h"

class SampleTool : public FurTool {
  private:
    class ToWaveSequenceTool : public SubTool {
      private:
        QLineEdit samplePath, insPath;
        QSpinBox waveWidth, waveHeight;
        QComboBox insType;
        QPushButton sPathOpen, iPathOpen, submit;
        QCheckBox raw;

        QHBoxLayout sPathLayout, iPathLayout;

        QLabel status;

        QAudioDecoder* decoder;
        size_t sampleLength;
        unsigned char* audioBuffer;
      private slots:
        void openSampleFileDialog();
        void openInsFileDialog();

        void readToBuffer();

        void convertPrepare();
        void convertFillBuffer();
        void convert();
      public:
        ToWaveSequenceTool(QWidget* _parent);
    }* waveSeqTool;

  public:
    SampleTool(QWidget* parent);
    ~SampleTool();
};
