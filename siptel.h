#ifndef SIPTEL_H
#define SIPTEL_H

#include <QMainWindow>
#include <QList>

#include "buddy.h"


namespace Ui {
class Siptel;
}

class Siptel : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event);

public:
    explicit Siptel(QWidget *parent = 0);
    ~Siptel();

private slots:
    void on_addButton_clicked();
    void on_accountButton_clicked();

    void on_deleteButton_clicked();

    void on_editButton_clicked();

    void on_registerButton_clicked();

private:
    Ui::Siptel *ui;

    QString domain;
    QString username;
    QString password;
    QString transport;
    QString logfile;
    QString loglevel;
    bool IsSubscribe;
    bool IsPublish;
    int SipPort;
    bool SipOn;
    QList<Buddy *> buddies;

private:
    void setUpSip();
    void WriteSettings();
    void ReadSettings();
    bool realExit();
    Buddy* getBuddy(QString uri);
    void deleteBuddy(Buddy *buddy);
    Buddy* addNewBuddy(QString name, QString uri, bool presence);
    void subscribeBuddy(Buddy *buddy);
};

#endif // SIPTEL_H
