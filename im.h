#ifndef IM_H
#define IM_H

#include <QWidget>

namespace Ui {
class Im;
}

class Im : public QWidget
{
    Q_OBJECT

public:
    explicit Im(QWidget *parent = 0);
    ~Im();

    void new_incoming_im(QString from, QString text);
    void setHandle(QString newHandle);
    QString getHandle();

private:
    Ui::Im *ui;
    QString imHandle;

signals:
    void new_outgoing_im(QString to, QString text);
private slots:
    void on_sendButton_clicked();
};

#endif // IM_H
