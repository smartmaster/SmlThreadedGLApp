#include "smlthreadedopenglmainwindow.h"
#include "SmlSurfaceFormat.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    SmlSurfaceFormatUtils::SurfaceFormat();
    QApplication a(argc, argv);
    SmlThreadedOpenglMainWindow w;
    w.show();
    return a.exec();
}
