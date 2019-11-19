#ifndef SELECTUSER_H
#define SELECTUSER_H

#include <QDialog>

namespace Ui {
class SelectUser;
}

class SelectUser : public QDialog
{
    Q_OBJECT

public:
    explicit SelectUser(QWidget *parent = 0);
    ~SelectUser();

private:
    Ui::SelectUser *ui;
};

#endif // SELECTUSER_H
