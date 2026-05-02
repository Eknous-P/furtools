#include "configTool.h"

ConfigTool::ConfigReadTool::ConfigReadTool(QWidget* _parent) {
  parent = _parent;

  configPath.setParent(parent);
  textarea.setParent(parent);

  layout.addRow("Config Directory", &configPath);
  layout.addRow(&textarea);

  group.setParent(parent);
  group.setTitle("Read config");
  group.setLayout(&layout);
}

ConfigTool::ConfigTool(QWidget* parent):
  FurTool(parent) {
    configReadTool = new ConfigReadTool(parent);

    layout->addWidget(configReadTool->getGroup());
}

ConfigTool::~ConfigTool() {
  delete configReadTool;
}
