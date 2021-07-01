#include "BiopsyTilerExplorer.h"
#include <QApplication>
#include <Chronograph.h>

int main(int argc, char *argv[])
{
    Chronograph::setOutputFile("TimeLog.txt");

    QApplication app(argc, argv);
    BiopsyTilerExplorer explorerWindow;
    explorerWindow.show();
    return app.exec();
}
