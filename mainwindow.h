#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include <pjsua-lib/pjsua.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setUpSip();

private slots:
    void on_loginB_clicked();

private:
    Ui::MainWindow *ui;
    pjsua_acc_id acc_id;
    pj_status_t status;
};

#endif // MAINWINDOW_H
