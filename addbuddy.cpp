#include "addbuddy.h"
#include "ui_addbuddy.h"

AddBuddy::AddBuddy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddBuddy)
{
    ui->setupUi(this);
}

AddBuddy::~AddBuddy()
{
    delete ui;
}

QString AddBuddy::getName()
{
    return ui->nameEdit->text();
}

bool AddBuddy::getPresence()
{
    if (ui->presenceBox->checkState() == Qt::Unchecked)\
        return false;
    else
        return true;
}

QString AddBuddy::getUri()
{
    return ui->uriEdit->text();
}

