#ifndef SIPTEL_H
#define SIPTEL_H

#include <QMainWindow>

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

private:
    void WriteSettings();
    void ReadSettings();
    bool realExit();
};

#endif // SIPTEL_H
