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

private:
    Ui::AddBuddy *ui;
};

#endif // ADDBUDDY_H
