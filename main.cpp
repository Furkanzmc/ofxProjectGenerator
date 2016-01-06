#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("ofProjectGenerator");
    QApplication::setApplicationVersion("0.2");
    QApplication::setOrganizationName("Furkanzmc");

    MainWindow w;
    w.show();

    return a.exec();
}
