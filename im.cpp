#include "im.h"
#include "ui_im.h"

Im::Im(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Im)
{
    ui->setupUi(this);
}

Im::~Im()
{
    delete ui;
}

void Im::new_incoming_im(QString from, QString text)
{
    if (!text.isEmpty()) {
        ui->historyEdit->append(from + QString(": ") + text);
    }
    ui->textEdit->setFocus();

}

void Im::setHandle(QString newHandle)
{
    this->imHandle = newHandle;
}


QString Im::getHandle()
{
    return this->imHandle;
}

void Im::on_sendButton_clicked()
{
    emit new_outgoing_im(this->imHandle, ui->textEdit->toPlainText());
    ui->historyEdit->append(QString("<i>you: </i>") + ui->textEdit->toPlainText());
    ui->textEdit->clear();
    ui->textEdit->setFocus();
}

