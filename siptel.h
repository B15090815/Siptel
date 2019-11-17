#ifndef SIPTEL_H
#define SIPTEL_H

#include <QMainWindow>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>

#include "buddy.h"
#include "im.h"
#include "callfunc.h"

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

signals:
    void shuttingDown();

public slots:
//    void dump_log_message(QString text);
    void new_incoming_im(QString from, QString text);
    void new_outgoing_im(QString to, QString text);
    void buddy_double_clicked(QListWidgetItem *item);
    void buddy_state(int buddy_id);
    void reg_state_slot(int acc_id_cb);
    void local_status_changed(QString text);
    void setCallState(QString);
    void setCallButtonText(QString);


private slots:
    void on_addButton_clicked();
    void on_accountButton_clicked();

    void on_deleteButton_clicked();

    void on_editButton_clicked();

    void on_registerButton_clicked();

    void on_callButton_clicked();

    void on_holdButton_clicked();

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
    bool onHold;
    bool IsLogin;
    QList<Buddy *> buddies;
    QList<Im*> imWindowList;
    CallFunc *pjCallback;

private:
    void setUpSip();
    void WriteSettings();
    void ReadSettings();
    bool realExit();
    Buddy* getBuddy(QString uri);
    QListWidgetItem* getBuddyItem(QString uri);
    void deleteBuddy(Buddy *buddy);
    Buddy* addNewBuddy(QString name, QString uri, bool presence);
    void subscribeBuddy(Buddy *buddy);
    void shutDownSip();
    void sendIm(QString,QString);
    int initializeSip();
};

#endif // SIPTEL_H
