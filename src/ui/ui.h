#ifndef FURTOOLS_H
#define FURTOOLS_H

#include <QWidget>
#include <QTabBar>

#include "tool.h"
#include "tools/sampleTool.h"
#include "tools/configTool.h"
#include "tools/utilsTool.h"

class FurToolsUI : public QWidget {
  private:
    QTabBar* tabBar;
    QVBoxLayout* layout;
    QFrame* toolsFrame;

    FurTool* currentTool;

    int currentTab;
    void switchTab();
  public:
    explicit FurToolsUI(QWidget* parent);
    ~FurToolsUI() override;
};

#endif // FURTOOLS_H
