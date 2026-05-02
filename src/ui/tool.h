#ifndef _TOOL_H
#define _TOOL_H

#include <QWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QCheckBox>

class FurTool : public QWidget {
  protected:
    QVBoxLayout* layout;
  public:
    FurTool(QWidget* parent);
    virtual ~FurTool();
};

class SubTool : public QWidget {
  protected:
    QWidget* parent;
    QGroupBox group;
    QFormLayout layout;
  public:
    QGroupBox* getGroup();
};

#endif
