#include "sampleTool.h"
#include <cstdio>
#include <qaudiodecoder.h>
#include <qaudioformat.h>
#include <qtypes.h>

template<typename T>
void writeInt(T a, FILE* f) {
  fwrite(&a, sizeof(T), 1, f);
}

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
      status.setText("failed to open sample file!");
      return;
    }
    fseek(sample, 0, SEEK_END);
    sampleLength=ftell(sample);
    rewind(sample);
    audioBuffer = new unsigned char[sampleLength];
    fwrite(audioBuffer, 1, sampleLength, sample);
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

    FILE* ins = fopen(insPath.text().toUtf8().data(), "wb");
    if (!ins) {
      status.setText("failed to save instrument!");
      return;
    }
    fwrite("FINS", 1, 4, ins);
    writeInt<unsigned short>(233, ins); // version
    const int insTypeI=insType.currentData().toInt();
    writeInt<unsigned short>(insTypeI, ins); // type

    // wave macro
    int wavesNum=ceil((float)sampleLength/waveWidthI);
    printf("will make 0x%x waves\n",wavesNum);
    fwrite("MA", 1, 2, ins);
    writeInt<unsigned short>(10+wavesNum+1, ins); // macro block length
    writeInt<unsigned short>(8, ins); // macro header length
    writeInt<unsigned char>(3, ins); // wave macro
    writeInt<unsigned char>(wavesNum, ins); // macro length
    writeInt<unsigned char>(255, ins); // loop
    writeInt<unsigned char>(255, ins); // release
    writeInt<unsigned char>(0, ins); // mode (sequence)
    writeInt<unsigned char>(0b0000001, ins); // flags
    writeInt<unsigned char>(0, ins); // delay
    writeInt<unsigned char>(1, ins); // speed
    for (int i=0; i<wavesNum; i++) {
      writeInt<unsigned char>(i, ins);
    }
    writeInt<unsigned char>(255, ins); // stop

    if (insTypeI==4) { // extra data for the gen. sample type
      fwrite("SM", 1, 2, ins);
      writeInt<unsigned short>(4, ins);
      writeInt<unsigned short>(0, ins);
      writeInt<unsigned char>(4, ins);
      writeInt<unsigned char>(waveWidthI-1, ins);
    }

    // wave list
    fwrite("LW", 1, 2, ins);
    writeInt<unsigned short>(wavesNum*3+2, ins); // block size
    writeInt<unsigned short>(wavesNum, ins);
    for (int i=0; i<wavesNum; i++) {
      writeInt<unsigned short>(i, ins);
    }
    unsigned int waveListPtr=ftell(ins);
    for (int i=0; i<wavesNum; i++) {
      writeInt<unsigned int>(0, ins);
    }
    writeInt<unsigned char>(0, ins);
    fwrite("EN", 1, 2, ins); // ins data end

    // waves
    size_t i=0;
    unsigned char* bufferPtr=audioBuffer;
    for (int iter=0;i<sampleLength;iter++) {
      unsigned int ptr=ftell(ins);
      int width = sampleLength - waveWidthI*iter;
      if (width>256) width=256;
      printf("writing wave %d... (width: %d)\n", iter, width);
      fwrite("WAVE", 1, 4, ins); // wave data
      writeInt<unsigned int>(13+width, ins); // block size
      writeInt<unsigned char>(0,ins); // name
      writeInt<unsigned int>(width, ins);
      writeInt<unsigned int>(0, ins);
      writeInt<unsigned int>(waveHeightI-1, ins);

      for (int j=0; j<width; j++, i++) {
        int w = waveHeightI*(*bufferPtr++)/256.f;
        writeInt<unsigned int>(w, ins);
      }
      // write pointer to list
      fseek(ins, waveListPtr, SEEK_SET);
      writeInt<unsigned int>(ptr, ins);
      waveListPtr+=4;
      fseek(ins, 0, SEEK_END);
    }
    // writeInt<unsigned char>(0, ins);
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
  waveHeight.setMaximum(256);
  waveHeight.setValue(256);

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
