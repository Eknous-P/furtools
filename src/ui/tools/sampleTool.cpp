#include "sampleTool.h"
#include <qaudiodecoder.h>
#include <qaudioformat.h>


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
  
  if (raw.checkState()) {
    FILE* sample = fopen(samplePath.text().toUtf8().data(), "rb");
    if (!sample) {
      status.setText("Failed to open sample file!");
      return;
    }
    sampleLength = getFileSize(sample);
    audioBuffer = new unsigned char[sampleLength];
    if (!audioBuffer) {
      status.setText("Failed to allocate sample buffer!");
      return;
    }
    fread(audioBuffer, 1, sampleLength, sample);
    printf("audio buffer: %p\n", audioBuffer);
    fclose(sample);
    convert();
  } else {
    QAudioFormat fmt;
    fmt.setChannelCount(1);
    fmt.setSampleFormat(QAudioFormat::UInt8);
    decoder = new QAudioDecoder(this);
    decoder->setAudioFormat(fmt);
    decoder->setSource(samplePath.text());
    connect(decoder, &QAudioDecoder::bufferReady, this, &SampleTool::ToWaveSequenceTool::convertFillBuffer);
    decoder->start();
    switch (decoder->error()) {
      case QAudioDecoder::NoError:
        break;
      default:
        status.setText(decoder->errorString());
        decoder->stop();
        delete decoder;
        break;
    }
  }
}

void SampleTool::ToWaveSequenceTool::convertFillBuffer() {
  auto data = decoder->read();
  sampleLength=data.sampleCount();
  audioBuffer = new unsigned char[sampleLength];
  memcpy(audioBuffer, data.data<quint8>(), sampleLength);
  printf("audio buffer: %p\n", audioBuffer);
  convert();
}

void SampleTool::ToWaveSequenceTool::convert() {
  if (!audioBuffer) {
    status.setText("buffer error!");
    return;
  }
  if (sampleLength>65536) {
    status.setText("audio too large!");
  } else {
    printf("audio buffer: %p\n", audioBuffer);
    printf("decoder: read %lu samples\n", sampleLength);
    int waveWidthI=waveWidth.value(),
        waveHeightI=waveHeight.value();
    printf("saving waves as [%d:%d]\n",waveWidthI,waveHeightI);

    int waves = ceil((float)sampleLength/waveWidthI);

    Furnace::Instrument inst(233, Furnace::InsTypes(insType.currentData().toInt()));
    Furnace::InsFeatureMacro macroFeature(233);
    Furnace::InsFeatureMacro::Macro macroData;
    macroData.code = Furnace::MacroWave;
    macroData.length = waves;
    macroData.loop = macroData.release = 255;
    macroData.mode = macroData.type = Furnace::MacroTypeNormal;
    macroData.open = 1;
    macroData.instantRelease = 0;
    macroData.wordSize = Furnace::MacroWord_S16;
    macroData.delay = 0;
    macroData.speed = 1;
    for (int i=0; i<waves; i++) {
      macroData.data[i] = i;
    }
    macroFeature.macroEntries = {macroData};
    inst.addFeature(&macroFeature);
    Furnace::InsFeatureWaveList waveFeature(233);
    size_t i=0;
    for (int j=0; j<waves; j++) {
      waveFeature.waveIndexes.push_back(j);
      Furnace::Wavetable w(waveWidthI, waveHeightI);
      int k=0;
      for (; k<waveWidthI; k++) {
        w.setData(k, waveHeightI*(float)audioBuffer[i++]/256.f);
        if (i>sampleLength) break;
      }
      w.setWidth(k);
      waveFeature.waves.push_back(w);
    }
    inst.addFeature(&waveFeature);
    

    FILE* ins = fopen(insPath.text().toUtf8().data(), "wb");
    if (!ins) {
      status.setText("failed to save instrument!");
      return;
    }
    inst.saveFile(ins);
    fclose(ins);

    status.setText("success!");
  }
  delete[] audioBuffer;
  audioBuffer = NULL;
  if (!raw.checkState()) {
    delete decoder;
  }
}

SampleTool::ToWaveSequenceTool::ToWaveSequenceTool(QWidget* _parent) {
  audioBuffer = NULL;
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

  QIcon openpix=QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen);

  sPathOpen.setIcon(openpix);
  connect(&sPathOpen, &QPushButton::clicked, this, &SampleTool::ToWaveSequenceTool::openSampleFileDialog);
  sPathLayout.addWidget(&samplePath);
  sPathLayout.addWidget(&sPathOpen);

  iPathOpen.setIcon(openpix);
  connect(&iPathOpen, &QPushButton::clicked, this, &SampleTool::ToWaveSequenceTool::openInsFileDialog);
  iPathLayout.addWidget(&insPath);
  iPathLayout.addWidget(&iPathOpen);

  raw.setParent(parent);

  submit.setParent(parent);
  submit.setText("Convert");
  connect(&submit, &QPushButton::clicked, this, &SampleTool::ToWaveSequenceTool::convertPrepare);

  status.setParent(parent);
  status.setText("");

  layout.addRow("Sample", &sPathLayout);
  layout.addRow("Sample is raw data", &raw);
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
