#include "tool.h"

class UtilsTool : public FurTool {
  private:
    class FileInterleaveTool : public SubTool {
      private:
        QLineEdit in1Path, in2Path, outPath;
        QPushButton in1Open, in2Open, outOpen;
        QHBoxLayout in1Layout, in2Layout, outLayout;

        QPushButton submit;
        QLabel status;
      private slots:
        void openIn1FileDialog();
        void openIn2FileDialog();
        void openOutFileDialog();

        void interleave();
      public:
        FileInterleaveTool(QWidget* _parent);
    }* fileInterleaveTool;
  
  public:
    UtilsTool(QWidget* parent);
    ~UtilsTool();
};
