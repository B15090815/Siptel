#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <QDialog>
#include <pjsua-lib/pjsua.h>
namespace Ui {
class Account;
}

class Account : public QDialog
{
    Q_OBJECT

public:
    explicit Account(QWidget *parent = 0);
    ~Account();


    QString getDomain();
    QString getUsername();
    QString getPassword();
    QString getTransport();
    QString getLogFile();
    QString getLogLevel();
    int getLocalPort();
    bool getSubscribe();
    bool getPublish();

    void setDomain(QString domain);
    void setUsername(QString username);
    void setPassword(QString password);
    void setTransport(QString tansport);
    void setLogFile(QString logfile);
    void setLogLevel(QString level);
    void setLocalPort(int port);
    void setSubscribe(bool IsSubscribe);
    void setPublish(bool IsPublish);
private slots:
    void on_logFilePushButton_clicked();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::Account *ui;


};
#endif // ACCOUNT_H
