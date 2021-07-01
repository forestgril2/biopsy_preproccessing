#include "BiopsyTilerExplorer.h"
#include <QApplication>
#include <Chronograph.h>

static const std::string kDefaultCellPositionsJsonPath("G:\\Shared drives\\MathPath\\03 Execute\\User Stories\\Preprocessing\\p55_post.json");
static const std::string kDefaultAnnotationsJsonPath("G:\\Shared drives\\MathPath\\03 Execute\\User Stories\\Preprocessing\\p55_poly_post.json");

int main(int argc, char *argv[])
{
    Chronograph::setOutputFile("TimeLog.txt");

    QApplication app(argc, argv);
    BiopsyTilerExplorer explorerWindow(kDefaultCellPositionsJsonPath, kDefaultAnnotationsJsonPath);
    explorerWindow.show();
    return app.exec();
}
