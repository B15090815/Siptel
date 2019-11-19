#include "selectuser.h"
#include "ui_selectuser.h"

SelectUser::SelectUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectUser)
{
    ui->setupUi(this);
}

SelectUser::~SelectUser()
{
    delete ui;
}
