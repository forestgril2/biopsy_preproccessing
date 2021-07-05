#include "BiopsyTilerExplorer.h"
#include <QApplication>
#include <Chronograph.h>

static const std::string kDefaultCellPositionsJsonPath("G:\\Shared drives\\MathPath\\03 Execute\\User Stories\\Preprocessing\\p55_post.json");
static const std::string kDefaultAnnotationsJsonPath("G:\\Shared drives\\MathPath\\03 Execute\\User Stories\\Preprocessing\\p55_poly_post.json");

int main(int argc, char *argv[])
{
    std::cout << " ### BiopsyTilerExplorer, start" << std::endl;

    Chronograph::setOutputFile("TimeLog.txt");

    if(argc != 3)
    {
        std::cout << " ### ERROR BiopsyTilerExplorer: wrong or missing input parameters. Exiting." << std::endl;
        exit(0);
    }

    std::string positionsPath(argv[1]);
    std::string annotationsPath(argv[2]);

    std::cout << " ### BiopsyTilerExplorer, positionsPath: " << positionsPath << std::endl;
    std::cout << " ### BiopsyTilerExplorer, input parameters: " << annotationsPath <<std::endl;

    QApplication app(argc, argv);
    BiopsyTilerExplorer explorerWindow(positionsPath, annotationsPath);
    explorerWindow.show();
    return app.exec();
}
