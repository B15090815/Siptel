#ifndef SELECTUSER_H
#define SELECTUSER_H

#include <QDialog>
#include <QDir>
namespace Ui {
class SelectUser;
}

class SelectUser : public QDialog
{
    Q_OBJECT

public:
    explicit SelectUser(QWidget *parent = 0);
    ~SelectUser();

    QString getUser();
    void setUser(QString user);

private slots:
    void on_ConfirmUser_clicked();

    void on_AddUser_clicked();

private:
    Ui::SelectUser *ui;
    QString choosedUser;

    QString domain;
    QString username;
    QString password;
    QString transport;
    QString logfile;
    QString loglevel;
    bool IsSubscribe;
    bool IsPublish;
    int SipPort;

    QDir *ConfigDir;

    void StoreSettings();
    void ReadSettings();
    void WriteSettings();
};

#endif // SELECTUSER_H
