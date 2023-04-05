#include "mainwindow.h"
#include "controller.h"
#include <QApplication>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFile stylesheetFile("./themes/Eclippy.qss");
    stylesheetFile.open(QFile::ReadOnly);
    QString stylesheet = QLatin1String(stylesheetFile.readAll());
    a.setStyleSheet(stylesheet);

    w.show();

    return a.exec();
}
