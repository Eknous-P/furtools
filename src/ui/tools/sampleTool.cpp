#include "sampleTool.h"


void SampleTool::ToWaveSequenceTool::openSampleFileDialog() {
  QString path = QFileDialog::getOpenFileName(
    parent,
    tr("Open sample..."),
    QDir::currentPath(),
    QObject::tr(
      "All files (*.*)"
    )
  );

  samplePath.setText(path);
}

void SampleTool::ToWaveSequenceTool::openInsFileDialog() {
  QString path = QFileDialog::getSaveFileName(
    parent,
    tr("Save instrument as..."),
    QDir::currentPath(),
    QObject::tr(
      "Furnace instrument (*.fui)"
    )
  );

  insPath.setText(path);
}

void SampleTool::ToWaveSequenceTool::convertPrepare() {
  printf("opening file %s...\n", samplePath.text().toUtf8().data());
  
#ifdef WITH_QT_MULTIMEDIA
  if (raw.checkState()) {
#endif
    FILE* sample = fopen(samplePath.text().toUtf8().data(), "rb");
    if (!sample) {
      status.setText("Failed to open sample file!");
      return;
    }
    sampleLength = getFileSize(sample);
    sampleBuffer = new unsigned char[sampleLength];
    if (!sampleBuffer) {
      status.setText("Failed to allocate sample buffer!");
      return;
    }
    fread(sampleBuffer, 1, sampleLength, sample);
    fclose(sample);
    convert();
#ifdef WITH_QT_MULTIMEDIA
  } else {
    QAudioFormat fmt;
    fmt.setChannelCount(1);
    fmt.setSampleFormat(QAudioFormat::UInt8);
    decoder = new QAudioDecoder(this);
    decoder->setAudioFormat(fmt);
    decoder->setSource(samplePath.text());
    connect(decoder, qOverload<>(&QAudioDecoder::error), this, &SampleTool::ToWaveSequenceTool::handleDecoderError);
    connect(decoder, &QAudioDecoder::bufferReady, this, &SampleTool::ToWaveSequenceTool::convertFillBuffer);
    decoder->start();
  }
#endif
}

#ifdef WITH_QT_MULTIMEDIA
void SampleTool::ToWaveSequenceTool::handleDecoderError() {
  status.setText(decoder->errorString());
  decoder->stop();
  delete decoder;
  decoder=NULL;
}

void SampleTool::ToWaveSequenceTool::convertFillBuffer() {
  if (!decoder) return;
  auto data = decoder->read();
  sampleLength=data.sampleCount();
  sampleBuffer = new unsigned char[sampleLength];
  memcpy(sampleBuffer, data.data<quint8>(), sampleLength);
  convert();
}
#endif

void SampleTool::ToWaveSequenceTool::convert() {
  if (!sampleBuffer) {
    status.setText("Buffer error!");
    return;
  }
  if (sampleLength > 32768*256) {
    status.setText("Audio too large!");
  } else {
    printf("decoder: read %lu samples\n", sampleLength);
    int waveWidthI=waveWidth.value(),
        waveHeightI=waveHeight.value();
    printf("saving waves as [%d:%d]\n",waveWidthI,waveHeightI);

    int waveCount = ceil((float)sampleLength/waveWidthI);

    const Furnace::Version fileVersion = 233;

    Furnace::Instrument inst(fileVersion, Furnace::InsTypes(insType.currentData().toInt()));
    Furnace::InsFeatureMacro macroFeature(fileVersion);
    Furnace::InsFeatureMacro::Macro macroData;
    macroData.code = Furnace::MacroWave;
    macroData.length = waveCount;
    macroData.loop = macroData.release = 255;
    macroData.mode = macroData.type = Furnace::MacroTypeNormal;
    macroData.open = 1;
    macroData.instantRelease = 0;
    macroData.wordSize = Furnace::MacroWord_S16;
    macroData.delay = 0;
    macroData.speed = 1;
    for (int i=0; i<waveCount; i++) {
      macroData.data[i] = i;
    }
    macroFeature.macroEntries = {macroData};
    inst.addFeature(&macroFeature);
    Furnace::InsFeatureWaveList waveFeature(fileVersion);
    size_t i=0;
    for (int j=0; j<waveCount; j++) {
      waveFeature.waveIndexes.push_back(j);
      Furnace::Wavetable w(waveWidthI, waveHeightI);
      for (int k=0; k<waveWidthI; k++) {
        w.setData(k, waveHeightI*(float)sampleBuffer[i++]/255.f);
        if (i>sampleLength) {
          w.setWidth(k);
          break;
        }
      }
      waveFeature.waves.push_back(w);
    }
    inst.addFeature(&waveFeature);
    
    FILE* ins = fopen(insPath.text().toUtf8().data(), "wb");
    if (!ins) {
      status.setText("Failed to save instrument!");
      return;
    }
    inst.saveFile(ins);
    fclose(ins);

    status.setText("Success!");
  }
  delete[] sampleBuffer;
  sampleBuffer = NULL;
#ifdef WITH_QT_MULTIMEDIA
  if (!raw.checkState()) {
    if (decoder) {
      delete decoder;
      decoder = NULL;
    }
  }
#endif
}

SampleTool::ToWaveSequenceTool::ToWaveSequenceTool(QWidget* _parent) {
  sampleBuffer = NULL;
  parent = _parent;
  samplePath.setParent(parent);
  insPath.setParent(parent);

  waveWidth.setParent(parent);
  waveWidth.setMinimum(1);
  waveWidth.setMaximum(256);
  waveWidth.setValue(256);

  waveHeight.setParent(parent);
  waveHeight.setMinimum(1);
  waveHeight.setMaximum(255);
  waveHeight.setValue(255);

  insType.setParent(parent);

  insType.addItem("Generic Sample", 4);
  insType.addItem("PC Engine", 5);
  insType.addItem("Namco 163", 17);

  QIcon openIcon=QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen);

  sPathOpen.setIcon(openIcon);
  connect(&sPathOpen, &QPushButton::clicked, this, &SampleTool::ToWaveSequenceTool::openSampleFileDialog);
  sPathLayout.addWidget(&samplePath);
  sPathLayout.addWidget(&sPathOpen);

  iPathOpen.setIcon(openIcon);
  connect(&iPathOpen, &QPushButton::clicked, this, &SampleTool::ToWaveSequenceTool::openInsFileDialog);
  iPathLayout.addWidget(&insPath);
  iPathLayout.addWidget(&iPathOpen);

#ifdef WITH_QT_MULTIMEDIA
  raw.setParent(parent);
#endif

  submit.setParent(parent);
  submit.setText("Convert");
  connect(&submit, &QPushButton::clicked, this, &SampleTool::ToWaveSequenceTool::convertPrepare);

  status.setParent(parent);
#ifdef WITH_QT_MULTIMEDIA
  status.setText("");
#else
  status.setText("File will be processed as raw data");
#endif

  layout.addRow("Sample", &sPathLayout);
#ifdef WITH_QT_MULTIMEDIA
  layout.addRow("Sample is raw data", &raw);
#endif
  layout.addRow("Width", &waveWidth);
  layout.addRow("Height", &waveHeight);
  layout.addRow("Instrument type", &insType);
  layout.addRow("Instrument", &iPathLayout);
  layout.addRow(&submit);
  layout.addRow(&status);

  group.setParent(parent);
  group.setTitle("Convert to wavetable sequence");
  group.setLayout(&layout);
}

SampleTool::SampleTool(QWidget* parent):
  FurTool(parent) {

  waveSeqTool = new ToWaveSequenceTool(this);

  layout->addWidget(waveSeqTool->getGroup());
}

SampleTool::~SampleTool() {
  delete waveSeqTool;
}
