#include "TubeServer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TubeServer w;
    w.show();

    return a.exec();
}
