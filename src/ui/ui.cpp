#include "ui.h"

void FurToolsUI::switchTab() {
  currentTab=tabBar->currentIndex();
  if (currentTool) delete currentTool;
  switch (currentTab) {
    case 0:
      currentTool = new SampleTool(toolsFrame);
      break;
    case 1:
      currentTool = new ConfigTool(toolsFrame);
      break;
    case 2:
      currentTool = new UtilsTool(toolsFrame);
      break;
  }
}

FurToolsUI::FurToolsUI(QWidget *parent):
  QWidget(parent) {
    currentTool = NULL;
    tabBar = new QTabBar(this);
    tabBar->addTab("Sample Tools");
    tabBar->addTab("Config Tools");
    tabBar->addTab("Misc. Utilities");

    connect(tabBar, &QTabBar::currentChanged, this, &FurToolsUI::switchTab);

    layout = new QVBoxLayout(this);
    toolsFrame = new QFrame(this);

    layout->addWidget(tabBar);
    layout->addWidget(toolsFrame);

    switchTab();
}

FurToolsUI::~FurToolsUI() {
  delete tabBar;
  delete layout;
  delete currentTool;
}
