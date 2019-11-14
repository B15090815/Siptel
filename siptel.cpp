#include "siptel.h"
#include "ui_siptel.h"
#include "addbuddy.h"
#include "account.h"

#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>




Siptel::Siptel(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Siptel)
{
    ui->setupUi(this);

    SipOn = false;

//    domain = "iptel.com";
//    username = "crx";
//    password = "crx0819";
//    loglevel = "1";
//    logfile = "/home/crx/master/sip_log.txt";
//    SipPort = 5056;
//    transport = "UDP";
//    IsSubscribe = true;
//    IsPublish = true;
    ReadSettings();
}

Siptel::~Siptel()
{
    delete ui;
}


bool Siptel::realExit() {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::question(this, tr("Siptel"),
            tr("Do you really want to exit?"), QMessageBox::Yes
                    | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        return true;
    } else {
        return false;
    }
}


void Siptel::closeEvent(QCloseEvent *event)
{
//    WriteSettings();
//    if (realExit()) {
////        WriteSettings();
//    } else {
//        event->ignore();
//    }

}

void Siptel::ReadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Bupt2019",
            "Siptel");
    settings.setFallbacksEnabled(false);

    settings.beginGroup("SIPAccount");
    domain = settings.value("domain").toString();
    username = settings.value("username").toString();
    password = settings.value("password").toString();
    transport = settings.value("transport").toString();
    logfile = settings.value("logFile").toString();
    loglevel = settings.value("logLevel").toString();
    SipPort = settings.value("sipPort").toInt();
    IsSubscribe = settings.value("subscribe").toBool();
    IsPublish = settings.value("publish").toBool();
    settings.endGroup();

//    settings.beginGroup("BuddyList");
//    int count = settings.beginReadArray("buddy");
//    for (int i = 0; i < count; i++) {
//        settings.setArrayIndex(i);
//        addNewBuddy(settings.value("Name").toString(),
//                settings.value("URI").toString(),
//                settings.value("Presence").toBool());
//    }
//    settings.endArray();
//    settings.endGroup();
}

void Siptel::WriteSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Bupt2019",
            "Siptel");

    settings.beginGroup("SIPAccount");
    settings.setValue("domain", domain);
    settings.setValue("username", username);
    settings.setValue("password", password);
    settings.setValue("transport", transport);
    settings.setValue("logFile", logfile);
    settings.setValue("logLevel", loglevel);
    settings.setValue("sipPort", SipPort);
    settings.setValue("subscribe", IsSubscribe);
    settings.setValue("publish", IsPublish);
    settings.endGroup();

//    settings.beginGroup("BuddyList");
//    settings.beginWriteArray("buddy");


//    Buddy * buddy;
//    int i=0;
//    while (buddies.size()) {
//        buddy = buddies.takeFirst();
//        settings.setArrayIndex(i);
//        settings.setValue("Name", buddy->name);
//        settings.setValue("Presence", buddy->presence);
//        settings.setValue("URI", buddy->uri);
//        i++;
//        delete buddy;
//    }
//    settings.endArray();
//    settings.endGroup();

    settings.sync();

}


void Siptel::on_addButton_clicked()
{
    AddBuddy *add;
    add = new AddBuddy();
    add->show();
}

void Siptel::on_accountButton_clicked()
{
    Account *acc = new Account(this);
    int change = 0;
    acc->setDomain(this->domain);
    acc->setUsername(this->username);
    acc->setPassword(this->password);
    acc->setTransport(this->transport);
    acc->setLogFile(this->logfile);
    acc->setLogLevel(this->loglevel);
    acc->setLocalPort(this->SipPort);
    acc->setSubscribe(this->IsSubscribe);
    acc->setPublish(this->IsPublish);


    QString temp;
    if (acc->exec()) {}

        temp = acc->getDomain().trimmed();
        if (temp != this->domain) {
            this->domain = temp;
            change++;
        }

        temp = acc->getUsername().trimmed();
        if (temp != this->username) {
            this->username = temp;
            change++;
        }

        temp = acc->getPassword().trimmed();
        if (temp != this->password) {
            this->password = temp;
            change++;
        }

        temp = acc->getTransport().trimmed();
        if (temp != this->transport) {
            this->transport = temp;
            change++;
        }

        temp = acc->getLogFile().trimmed();
        if (temp != this->logfile) {
            this->logfile = temp;
            change++;
        }

        temp = acc->getLogLevel();
        if (temp != this->loglevel) {
            this->loglevel = temp;
            change++;
        }

        int p = acc->getLocalPort();
        if (p != this->SipPort) {
            this->SipPort = p;
            change++;
        }

        bool tb = acc->getSubscribe();
        if (tb != this->IsSubscribe) {
            this->IsSubscribe = tb;
            change++;
        }

        tb = acc->getPublish();
        if (tb != this->IsPublish) {
            this->IsPublish = tb;
            change++;
        }

        if (change > 0) {
            if (SipOn) {
                // restart sip
            }
        }




//    qDebug() << acc->getDomain();


//    acc.exec();
//    acc = new  Account(this);
//    if (acc->show())
//    acc->show();
}
