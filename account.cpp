#include "account.h"
#include "ui_account.h"

#include <QFileDialog>

Account::Account(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Account)
{
    ui->setupUi(this);
}

Account::~Account()
{
    delete ui;
}

QString Account::getDomain()
{
    return ui->domainEdit->text();
}

QString Account::getUsername() {
    return ui->usernameEdit->text();
}

QString Account::getPassword() {
    return ui->passwordEdit->text();
}

QString Account::getTransport() {
    return ui->transportComboBox->currentText();
}

QString Account::getLogFile() {
    return ui->logFileEdit->text();
}

QString Account::getLogLevel() {
    return ui->logLevelComboBox->currentText();
}

int Account::getLocalPort()
{
    return ui->sipPortSpinBox->value();
}

bool Account::getPublish()
{
    if (ui->publishBox->checkState() == Qt::Unchecked)
        return false;
    else
        return true;
}

bool Account::getSubscribe()
{
    if (ui->subscribeBox->checkState() == Qt::Unchecked)
        return false;
    else
        return true;
}

void Account::setDomain(QString domain)
{
    ui->domainEdit->setText(domain);
}

void Account::setUsername(QString username)
{
    ui->usernameEdit->setText(username);
}

void Account::setPassword(QString password)
{
    ui->passwordEdit->setText(password);
}

void Account::setLogFile(QString logfile)
{
    ui->logFileEdit->setText(logfile);
}

void Account::setLogLevel(QString level)
{
    ui->logLevelComboBox->setCurrentIndex(ui->logLevelComboBox->findText(level));
}

void Account::setTransport(QString transport)
{
    ui->transportComboBox->setCurrentIndex(ui->transportComboBox->findText(transport));
}

void Account::setLocalPort(int port)
{
    ui->sipPortSpinBox->setValue(port);
}
void Account::setSubscribe(bool IsSubscribe)
{
    if (IsSubscribe) {
        ui->subscribeBox->setCheckState(Qt::Checked);
    } else {
        ui->subscribeBox->setCheckState(Qt::Unchecked);
    }
}

void Account::setPublish(bool IsPublish)
{
    if (IsPublish) {
        ui->publishBox->setCheckState(Qt::Checked);
    } else {
        ui->publishBox->setCheckState(Qt::Unchecked);
    }

}

void Account::on_logFilePushButton_clicked()
{
    QString  filename = QFileDialog::getSaveFileName(this,
            tr("Select log file"),
            this->getLogFile(),
            tr("Log Files (*.txt)"));
    if (!filename.isEmpty()) {
        ui->logFileEdit->setText(filename);
    }
}


void Account::on_okButton_clicked()
{
    this->close();
}

void Account::on_cancelButton_clicked()
{
    this->close();
}
