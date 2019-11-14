#include "mainwindow.h"
#include "siptel.h"
#include "im.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Im im;
//    im.show();
//    MainWindow w;
//    Account acc;
    Siptel sip;
    sip.show();
//    acc.show();
//    w.show();

    return a.exec();
}
