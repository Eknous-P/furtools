#include "tool.h"

FurTool::FurTool(QWidget* parent) {
  layout = new QVBoxLayout(parent);
}

FurTool::~FurTool() {
  delete layout;
}

QGroupBox* SubTool::getGroup() {
  return &group;
}
