#include "tool.h"
#ifdef WITH_QT_MULTIMEDIA
#include <QAudioDecoder>
#endif
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

#ifdef WITH_QT_MULTIMEDIA
        QAudioDecoder* decoder;
#endif
        size_t sampleLength;
        unsigned char* sampleBuffer;
      private slots:
        void openSampleFileDialog();
        void openInsFileDialog();

        void convertPrepare();
#ifdef WITH_QT_MULTIMEDIA
        void handleDecoderError();
        void convertFillBuffer();
#endif
        void convert();
      public:
        ToWaveSequenceTool(QWidget* _parent);
    }* waveSeqTool;

  public:
    SampleTool(QWidget* parent);
    ~SampleTool();
};
