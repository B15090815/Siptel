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

void AddBuddy::setName(QString name)
{
    ui->nameEdit->setText(name);
}

void AddBuddy::setPresence(bool presence)
{
    if (presence) {
        ui->presenceBox->setCheckState(Qt::Checked);
    } else {
        ui->presenceBox->setCheckState(Qt::Unchecked);
    }
}

void AddBuddy::setUri(QString uri)
{
    ui->uriEdit->setText(uri);
}
void AddBuddy::on_okButton_clicked()
{
    accept();
}

void AddBuddy::on_cancelButton_clicked()
{
    reject();
}
