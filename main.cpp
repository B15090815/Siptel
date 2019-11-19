#include "mainwindow.h"
#include "siptel.h"
#include "im.h"
#include "selectuser.h"
#include <QApplication>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SelectUser *user = new SelectUser();
    Siptel sip;
    if ((user->exec()) == QDialog::Accepted) {
        QString choosedUser = user->getUser();
        sip.setChoosedUser(choosedUser);
        sip.LoadSettings();
        sip.show();
    }
    delete user;

    return a.exec();
}
