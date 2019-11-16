#ifndef ADDBUDDY_H
#define ADDBUDDY_H

#include <QDialog>

namespace Ui {
class AddBuddy;
}

class AddBuddy : public QDialog
{
    Q_OBJECT

public:
    explicit AddBuddy(QWidget *parent = 0);
    ~AddBuddy();

    QString getName();
    QString getUri();
    bool getPresence();
    void setName(QString name);
    void setUri(QString uri);
    void setPresence(bool presence);

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::AddBuddy *ui;
};

#endif // ADDBUDDY_H
