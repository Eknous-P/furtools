#include "utilsTool.h"
#include "interleave.h"

void UtilsTool::FileInterleaveTool::openIn1FileDialog() {
  QString path = QFileDialog::getOpenFileName(
    parent,
    tr("Open File 1..."),
    QDir::currentPath(),
    QObject::tr(
      "All files (*.*)"
    )
  );
  in1Path.setText(path);
}

void UtilsTool::FileInterleaveTool::openIn2FileDialog() {
  QString path = QFileDialog::getOpenFileName(
    parent,
    tr("Open File 2..."),
    QDir::currentPath(),
    QObject::tr(
      "All files (*.*)"
    )
  );
  in2Path.setText(path);
}

void UtilsTool::FileInterleaveTool::openOutFileDialog() {
  QString path = QFileDialog::getSaveFileName(
    parent,
    tr("Choose out file..."),
    QDir::currentPath(),
    QObject::tr(
      "All files (*.*)"
    )
  );
  outPath.setText(path);
}

void UtilsTool::FileInterleaveTool::interleave() {
  if (in1Path.text().isEmpty()) {
    status.setText("File 1 not chosen!");
    return;
  }
  if (in2Path.text().isEmpty()) {
    status.setText("File 2 not chosen!");
    return;
  }
  if (outPath.text().isEmpty()) {
    status.setText("Out file not chosen!");
    return;
  }
  FILE *in1File=NULL, *in2File=NULL, *outFile=NULL;
  int err=-1;
  in1File = fopen(in1Path.text().toUtf8().data(), "rb");
  if (!in1File) {
    status.setText("Failed to open file 1!");
    goto finish;
  }
  in2File = fopen(in2Path.text().toUtf8().data(), "rb");
  if (!in2File) {
    status.setText("Failed to open file 2!");
    goto finish;
  }
  outFile = fopen(outPath.text().toUtf8().data(), "wb");
  if (!outFile) {
    status.setText("Failed to open out file!");
    goto finish;
  }

  err = Utils::fileInterleave(in1File, in2File, outFile);
  if (err==1) {
    status.setText("Input files not of the same size!");
  } else if (err==0) {
    status.setText("Success!");
  }

  finish:
  if (in1File) fclose(in1File);
  if (in2File) fclose(in2File);
  if (outFile) fclose(outFile);
  // safety against accidental clicks
  if (err==0) outPath.setText("");
}

UtilsTool::FileInterleaveTool::FileInterleaveTool(QWidget* _parent) {
  parent = _parent;
  in1Path.setParent(parent);
  in2Path.setParent(parent);
  outPath.setParent(parent);

  QIcon openIcon=QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen);
  in1Open.setIcon(openIcon);
  in2Open.setIcon(openIcon);
  outOpen.setIcon(openIcon);

  connect(&in1Open, &QPushButton::clicked, this, &FileInterleaveTool::openIn1FileDialog);
  connect(&in2Open, &QPushButton::clicked, this, &FileInterleaveTool::openIn2FileDialog);
  connect(&outOpen, &QPushButton::clicked, this, &FileInterleaveTool::openOutFileDialog);

  in1Layout.addWidget(&in1Path);
  in1Layout.addWidget(&in1Open);
  in2Layout.addWidget(&in2Path);
  in2Layout.addWidget(&in2Open);
  outLayout.addWidget(&outPath);
  outLayout.addWidget(&outOpen);

  submit.setParent(parent);
  submit.setText("Interleave");
  connect(&submit, &QPushButton::clicked, this, &FileInterleaveTool::interleave);

  status.setParent(parent);
  status.setText("");

  layout.addRow("File 1", &in1Layout);
  layout.addRow("File 2", &in2Layout);
  layout.addRow("Out file", &outLayout);
  layout.addRow(&submit);
  layout.addRow(&status);

  group.setParent(parent);
  group.setTitle("Interleave files");
  group.setLayout(&layout);
}

UtilsTool::UtilsTool(QWidget* parent):
  FurTool(parent) {
    fileInterleaveTool = new FileInterleaveTool(parent);

    layout->addWidget(fileInterleaveTool->getGroup());
}

UtilsTool::~UtilsTool() {
  delete fileInterleaveTool;
}
