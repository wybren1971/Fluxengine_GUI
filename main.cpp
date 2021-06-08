#include "mainwindow.h"

#include <QApplication>
#include <QProcess>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QCoreApplication::setApplicationVersion(QString(APP_VERSION));
    return a.exec();
}
