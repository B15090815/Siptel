#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "util.h"

#define THIS_FILE "siptel"

#include<QDebug>



static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
 pjsip_rx_data *rdata)
{
 pjsua_call_info ci;

 PJ_UNUSED_ARG(acc_id);
 PJ_UNUSED_ARG(rdata);

 pjsua_call_get_info(call_id, &ci);

 PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!",
 (int)ci.remote_info.slen,
 ci.remote_info.ptr));

 /* Automatically answer incoming calls with 200/OK */
 pjsua_call_answer(call_id, 200, NULL, NULL);
}

static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
 pjsua_call_info ci;

 PJ_UNUSED_ARG(e);

 pjsua_call_get_info(call_id, &ci);
 PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
 (int)ci.state_text.slen,
 ci.state_text.ptr));
}

/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id)
{
 pjsua_call_info ci;

 pjsua_call_get_info(call_id, &ci);

 if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
 // When media is active, connect call to sound device.
 pjsua_conf_connect(ci.conf_slot, 0);
 pjsua_conf_connect(0, ci.conf_slot);
 }
}


static void error_exit(const char *title, pj_status_t status)
{
  pjsua_perror(THIS_FILE, title, status);
  pjsua_destroy();
  exit(1);
}




MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUpSip();
}

void MainWindow::setUpSip()
{
    status = pjsua_create();

    if (status != PJ_SUCCESS) {
        error_exit("Error in pjsua_create()", status);
    }

    {
        pjsua_config cfg;
        pjsua_logging_config log_cfg;
        pjsua_config_default(&cfg);
        cfg.cb.on_incoming_call = &on_incoming_call;
        cfg.cb.on_call_media_state = &on_call_media_state;
        cfg.cb.on_call_state = &on_call_state;
        pjsua_logging_config_default(&log_cfg);
        log_cfg.console_level = 1;
        log_cfg.log_filename = pj_str("./telsip.log");
        status = pjsua_init(&cfg, &log_cfg, NULL);
        if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
    }

    /* Add UDP transport. */
    {
        pjsua_transport_config cfg;
        pjsua_transport_config_default(&cfg);
        cfg.port = 5060;
        status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
        if (status != PJ_SUCCESS) error_exit("Error creating transport", status);
    }

    status = pjsua_start();
    if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);

}


MainWindow::~MainWindow()
{
    delete ui;
    pjsua_destroy();
}


void MainWindow::on_loginB_clicked()
{
    QString uname = ui->username->text();
    QString domain = ui->domain->text();
    QString passwd = ui->password->text();
    QString id = "sip:" + uname + "@" + domain;
    QString uri = "sip:" + domain;

    char ch_id[USERINFOLEN];
    char ch_uri[USERINFOLEN];
    char ch_name[USERINFOLEN];
    char ch_passwd[USERINFOLEN];

    pj_str_t uid = QstrToPstr(id,ch_id);
    pj_str_t reg_uri = QstrToPstr(uri,ch_uri);
    pj_str_t username = QstrToPstr(uname,ch_name);
    pj_str_t password = QstrToPstr(passwd,ch_passwd);

//    pj_str_t uid = pj_str(id.toLatin1().data());
//    pj_str_t reg_uri = pj_str(uri.toLatin1().data());
//    pj_str_t username = pj_str(uname.toLatin1().data());
//    pj_str_t password = pj_str(passwd.toLatin1().data());

    {
        pjsua_acc_config cfg;
        pjsua_acc_config_default(&cfg);
        cfg.id = uid;
        cfg.reg_uri = reg_uri;
        cfg.cred_count = 1;
        cfg.cred_info[0].realm = pj_str("*");
        cfg.cred_info[0].scheme = pj_str("digest");
        cfg.cred_info[0].username = username;
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg.cred_info[0].data = password;
        status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
        if (status != PJ_SUCCESS) error_exit("Error adding account", status);
        else qDebug() << endl << "login success";
    }


}
