#include "tool.h"
#include <QTextEdit>

class ConfigTool : public FurTool {
  private:
    class ConfigReadTool : public SubTool {
      private:
        QLineEdit configPath;
        QTextEdit textarea;
      public:
        ConfigReadTool(QWidget* _parent);
    }* configReadTool;
  public:
    ConfigTool(QWidget* parent);
    ~ConfigTool();
};
