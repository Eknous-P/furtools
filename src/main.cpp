#include <QApplication>
#include "ui.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  FurToolsUI window(NULL);
  window.resize(400,480);
  window.show();

  return app.exec();
}
