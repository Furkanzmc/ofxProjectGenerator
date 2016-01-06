#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("ofProjectGenerator");
    QApplication::setApplicationVersion("1.0.1");
    QApplication::setOrganizationName("Furkanzmc");

    MainWindow w;
    w.show();

    return a.exec();
}
