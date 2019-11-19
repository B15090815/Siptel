#include "selectuser.h"
#include "ui_selectuser.h"
#include "account.h"
#include <QSettings>
#include <QStringList>
#include <QDebug>
SelectUser::SelectUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectUser)
{
    ui->setupUi(this);
    SipPort = 5060;
    IsSubscribe = false;
    IsPublish = false;
    ConfigDir = new QDir(QCoreApplication::applicationDirPath());
    ConfigDir->cd("./config");
//    ConfigDir->absolutePath("config");
    ReadSettings();
}

SelectUser::~SelectUser()
{
    delete ui;
    delete ConfigDir;
}

QString SelectUser::getUser()
{
    return choosedUser;
}


void SelectUser::setUser(QString user)
{
    ui->UsercomboBox->addItem(user);
}

void SelectUser::ReadSettings()
{
    QString dir = ConfigDir->absoluteFilePath("Siptel");
    QSettings settings(dir,QSettings::IniFormat);

    settings.setFallbacksEnabled(false);
    settings.beginGroup("SIPAccount");
    QStringList user = settings.allKeys();
    for (int i=0;i<user.length();i++) {
        setUser(user.at(i));
    }
    settings.endGroup();
}

void SelectUser::WriteSettings()
{
    if (username.isEmpty() || domain.isEmpty()) {
        return;
    }
    QString dir = ConfigDir->absoluteFilePath("Siptel");
    QSettings settings(dir,QSettings::IniFormat);
    QString uri = username + QString(":") + domain;
    settings.beginGroup("SIPAccount");
    if (!settings.contains(uri)) {
        settings.setValue(uri, 0);
    }
    settings.endGroup();
    settings.sync();
}

void SelectUser::on_ConfirmUser_clicked()
{
    if (ui->UsercomboBox->count() == 0) {
        return;
    }
    choosedUser = ui->UsercomboBox->currentText();
    if (choosedUser == "")
        return;
//    WriteSettings();
    accept();
}

void SelectUser::on_AddUser_clicked()
{
    Account *acc = new Account(this);
    QString temp;
    if (acc->exec() == QDialog::Accepted) {

        temp = acc->getDomain().trimmed();
        if (temp != this->domain) {
            this->domain = temp;
        }

        temp = acc->getUsername().trimmed();
        if (temp != this->username) {
            this->username = temp;
        }

        temp = acc->getPassword().trimmed();
        if (temp != this->password) {
            this->password = temp;
        }

        temp = acc->getTransport().trimmed();
        if (temp != this->transport) {
            this->transport = temp;
        }

        temp = acc->getLogFile().trimmed();
        if (temp != this->logfile) {
            this->logfile = temp;
        }

        temp = acc->getLogLevel();
        if (temp != this->loglevel) {
            this->loglevel = temp;
        }

        int p = acc->getLocalPort();
        if (p != this->SipPort) {
            this->SipPort = p;
        }

        bool tb = acc->getSubscribe();
        if (tb != this->IsSubscribe) {
            this->IsSubscribe = tb;
        }

        tb = acc->getPublish();
        if (tb != this->IsPublish) {
            this->IsPublish = tb;
        }
   }

    if (username.isEmpty() || domain.isEmpty()) {
        return;
    }
    choosedUser = username + ":" +domain;
    ui->UsercomboBox->addItem(choosedUser);
    WriteSettings();
    StoreSettings();
}

void SelectUser::StoreSettings()
{
    QString dir = ConfigDir->absoluteFilePath(choosedUser);
    QSettings settings(dir,QSettings::IniFormat);
    settings.beginGroup("UserInfo");
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
    settings.sync();
}



