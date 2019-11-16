#include "siptel.h"
#include "ui_siptel.h"
#include "addbuddy.h"
#include "account.h"
#include "buddy.h"
#include "im.h"
#include "appinfo.h"
#include "pjcallback.h"
#include "util.h"

#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMutex>

extern "C" {
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjmedia-codec.h>
#include <pjsip.h>
#include <pjsip_simple.h>
#include <pjsip_ua.h>
#include <pjsua-lib/pjsua.h>

}


/* global structures for pjsip config */
pjsua_config cfg;
pjsua_logging_config log_cfg;
pjsua_media_config media_cfg;
pjsua_transport_config transport_cfg;
pjsua_transport_config rtp_cfg;
pjsua_buddy_config buddy_cfg;
pjsua_acc_config acc_cfg;
pjsua_acc_id acc_id;
pj_pool_t *pool;

QList<int> activeCalls;
QMutex activeCallsMutex;

Siptel::Siptel(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Siptel)
{
    ui->setupUi(this);
    SipOn = false;
    ReadSettings();
    setUpSip();
}

Siptel::~Siptel()
{
    delete ui;
    pjsua_destroy();
}

void Siptel::setUpSip()
{
    if (this->SipOn)
        return;

    pj_status_t status;
    status = pjsua_create();
    if (status != PJ_SUCCESS) {
        error("Error in pjsua_create()", status);
    }

    if (domain.isEmpty() || username.isEmpty() ) {
//        QMessageBox::StandardButton ret;
        QMessageBox::warning(
                        this,
                        tr(USER_AGENT),
                        tr("SIP account not configured! Please configure at least username and domain!"),
                        QMessageBox::Ok);
        return;
    }

    /**
     * configure the pjlib
     */
    {
        pjsua_config cfg;
        pjsua_logging_config log_cfg;
        pjsua_config_default(&cfg);
        cfg.cb.on_incoming_call = &on_incoming_call;
        cfg.cb.on_call_media_state = &on_call_media_state;
        cfg.cb.on_call_state = &on_call_state;

//        cfg.cb.on_pager = PjCallback::on_pager_wrapper;
//        cfg.cb.on_reg_state = PjCallback::on_reg_state_wrapper;
//        cfg.cb.on_buddy_state = PjCallback::on_buddy_state_wrapper;
//        cfg.cb.on_nat_detect = PjCallback::on_nat_detect_wrapper;

//        pjsua_logging_config_default(&log_cfg);
//        log_cfg.msg_logging = true;
//        log_cfg.console_level = logLevel.toUInt();
//        log_cfg.cb = PjCallback::logger_cb_wrapper;
//        log_cfg.decor = log_cfg.decor & ~PJ_LOG_HAS_NEWLINE;

//        pjsua_media_config_default(&media_cfg);
//        media_cfg.no_vad = true;
        // configure the log
        pjsua_logging_config_default(&log_cfg);
        log_cfg.console_level = this->loglevel.toInt();
        char fileName[STRLEN];

        log_cfg.log_filename = QstrToPstr(this->logfile,fileName,STRLEN);
        log_cfg.decor = log_cfg.decor & ~PJ_LOG_HAS_NEWLINE;
        //init the pjsua
        status = pjsua_init(&cfg, &log_cfg, NULL);
        if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
    }

    /* Add transport. */
    {
        pjsua_transport_config cfg;
        pjsua_transport_config_default(&cfg);
        cfg.port = this->SipPort;
        if (this->transport == "UDP") {
            status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
        } else if (this->transport == "TCP") {
            status = pjsua_transport_create(PJSIP_TRANSPORT_TCP, &cfg, NULL);
        }

        if (status != PJ_SUCCESS) error_exit("Error creating transport", status);
    }

    status = pjsua_start();
    if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);
}


bool Siptel::realExit() {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::question(this, tr("Siptel"),
            tr("Do you really want to exit?"), QMessageBox::Yes
                    | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        return true;
    } else {
        return false;
    }
}


void Siptel::closeEvent(QCloseEvent *event)
{
    WriteSettings();
//    if (realExit()) {
//        WriteSettings();
//    } else {
//        event->ignore();
//    }

}

void Siptel::ReadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Bupt2019",
            "Siptel");
    settings.setFallbacksEnabled(false);

    settings.beginGroup("SIPAccount");
    domain = settings.value("domain").toString();
    username = settings.value("username").toString();
    password = settings.value("password").toString();
    transport = settings.value("transport").toString();
    logfile = settings.value("logFile").toString();
    loglevel = settings.value("logLevel").toString();
    SipPort = settings.value("sipPort").toInt();
    IsSubscribe = settings.value("subscribe").toBool();
    IsPublish = settings.value("publish").toBool();
    settings.endGroup();

    settings.beginGroup("BuddyList");
    int count = settings.beginReadArray("buddy");
    for (int i = 0; i < count; i++) {
        settings.setArrayIndex(i);
        addNewBuddy(settings.value("Name").toString(),
                settings.value("URI").toString(),
                settings.value("Presence").toBool());
    }
    settings.endArray();
    settings.endGroup();
}

void Siptel::WriteSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Bupt2019",
            "Siptel");

    settings.beginGroup("SIPAccount");
    settings.setValue("domain", domain);
    settings.setValue("username", username);
    settings.setValue("password", password);
    settings.setValue("transport", transport);
    settings.setValue("logFile", logfile);
    settings.setValue("logLevel", loglevel);
    settings.setValue("sipPort", SipPort);
    settings.setValue("subscribe", IsSubscribe);
    settings.setValue("publish", IsPublish);
    settings.endGroup();

    settings.beginGroup("BuddyList");
    settings.beginWriteArray("buddy");
    Buddy * buddy;
    int i=0;
    while (!buddies.empty()) {
        buddy = buddies.takeFirst();
        settings.setArrayIndex(i);
        settings.setValue("Name", buddy->name);
        settings.setValue("Presence", buddy->presence);
        settings.setValue("URI", buddy->uri);
        i++;
        delete buddy;
    }
    settings.endArray();
    settings.endGroup();

    settings.sync();

}


Buddy* Siptel::addNewBuddy(QString name, QString uri, bool presence)
{
    if ((!name.isEmpty()) && (!uri.isEmpty())) {
        Buddy *buddy = new Buddy();
        buddy->name = name.trimmed();
        uri = uri.trimmed();
        if (!uri.startsWith("sip:",Qt::CaseInsensitive)) {
            uri = "sip:" + uri;
        }
        buddy->uri = uri;
        buddy->presence = presence;
        QListWidgetItem *item = new QListWidgetItem(buddy->name, ui->buddyList);
        item->setData(Qt::UserRole, buddy->uri);
//        if (presence) {
//            item->setIcon(QIcon(":/icons/unknown"));
//        }
        item->setToolTip(buddy->uri);
        buddies << buddy;
        return buddy;
    }
    return 0;
}

void Siptel::on_addButton_clicked()
{
    AddBuddy *add;
    add = new AddBuddy();
    if (add->exec() == QDialog::Accepted) {
        addNewBuddy(add->getName(),add->getUri(),add->getPresence());
    }

    delete add;
}

void Siptel::on_accountButton_clicked()
{
    Account *acc = new Account(this);
    int change = 0;
    acc->setDomain(this->domain);
    acc->setUsername(this->username);
    acc->setPassword(this->password);
    acc->setTransport(this->transport);
    acc->setLogFile(this->logfile);
    acc->setLogLevel(this->loglevel);
    acc->setLocalPort(this->SipPort);
    acc->setSubscribe(this->IsSubscribe);
    acc->setPublish(this->IsPublish);


    QString temp;
    if (acc->exec() == QDialog::Accepted) {

        temp = acc->getDomain().trimmed();
        if (temp != this->domain) {
            this->domain = temp;
            change++;
        }

        temp = acc->getUsername().trimmed();
        if (temp != this->username) {
            this->username = temp;
            change++;
        }

        temp = acc->getPassword().trimmed();
        if (temp != this->password) {
            this->password = temp;
            change++;
        }

        temp = acc->getTransport().trimmed();
        if (temp != this->transport) {
            this->transport = temp;
            change++;
        }

        temp = acc->getLogFile().trimmed();
        if (temp != this->logfile) {
            this->logfile = temp;
            change++;
        }

        temp = acc->getLogLevel();
        if (temp != this->loglevel) {
            this->loglevel = temp;
            change++;
        }

        int p = acc->getLocalPort();
        if (p != this->SipPort) {
            this->SipPort = p;
            change++;
        }

        bool tb = acc->getSubscribe();
        if (tb != this->IsSubscribe) {
            this->IsSubscribe = tb;
            change++;
        }

        tb = acc->getPublish();
        if (tb != this->IsPublish) {
            this->IsPublish = tb;
            change++;
        }

        if (change > 0) {
            if (SipOn) {
                // restart sip
            }
        }
   }
}

Buddy* Siptel::getBuddy(QString uri)
{
    Buddy *buddy;
    for(int i=0;i<buddies.size();i++) {
        buddy = buddies.at(i);
        if (buddy->uri == uri)
            return buddy;
    }
    return 0;
}

void Siptel::deleteBuddy(Buddy *buddy)
{
    QListWidgetItem *item;
    QList<Buddy*>::iterator itr;
    if (!buddy) return;
    for (itr = buddies.begin(); itr != buddies.end(); itr++) {
        if ((*itr) == buddy) {
            buddies.erase(itr);

            for (int i=0; i<ui->buddyList->count(); i++) {
                item = ui->buddyList->item(i);
                if (buddy->uri == item->data(Qt::UserRole).toString()) {
                    /* test if deleting automatically removes it from the list */
                    /* if yes, 			item=getBuddyItem(buddy->uri); can be used too */
                    delete item;
                }
            }

            delete buddy;
            break;
        }
     }
}

void Siptel::on_deleteButton_clicked()
{
    QListWidgetItem *curItem = ui->buddyList->currentItem();
    QMessageBox::StandardButton ret;
    ret = QMessageBox::question(this, tr("QjSimple"),
            tr("Do you really want to delete ") + curItem->text() + "?",
            QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return;
    }

    if (curItem) {
        Buddy *buddy;
        buddy = getBuddy(curItem->data(Qt::UserRole).toString());
        if (buddy) {
            deleteBuddy(buddy);
        }
    }
}

void Siptel::subscribeBuddy(Buddy *buddy)
{
    pj_status_t status;
    if (!buddy || !SipOn) return;
    pjsua_buddy_config_default(&buddy_cfg);
    buddy_cfg.subscribe = PJ_TRUE;
    QByteArray temp=buddy->uri.toLatin1();
    buddy_cfg.uri = pj_str(temp.data());
    status = pjsua_buddy_add(&buddy_cfg, &(buddy->buddy_id));
//    appLog("subscribeBuddy: Buddy id for " + buddy->name + "(" + buddy->uri + ")=" + QString::number(buddy->buddy_id));
    if (status != PJ_SUCCESS) {
//        error("Error adding buddy", status);
    }
    buddy->buddy_id_valid = true;
}

void Siptel::on_editButton_clicked()
{
    QListWidgetItem *curItem = ui->buddyList->currentItem();
    if (curItem) {
        Buddy *buddy;
        buddy = getBuddy(curItem->data(Qt::UserRole).toString());
        if (buddy) {
            AddBuddy dialog(this);
            dialog.setName(buddy->name);
            dialog.setUri(buddy->uri);
            dialog.setPresence(buddy->presence);
            if (dialog.exec()) {
                QString name = dialog.getName().trimmed();
                QString uri = dialog.getUri().trimmed();
                bool presence = dialog.getPresence();
                if ((name!=buddy->name) || (uri!=buddy->uri) || (presence!=buddy->presence)) {
                    /* buddy was changed */
//                    if (subscribe) {
//                        unsubscribeBuddy(buddy);
//                    }
                    deleteBuddy(buddy);
                    buddy = addNewBuddy(name, uri, presence);
//                    if (buddy && subscribe) {
//                        subscribeBuddy(buddy);
//                    }
                }
            }
        }
    }
}

void Siptel::on_registerButton_clicked()
{
    pj_status_t status;
    if (ui->registerButton->text() == "register") {

        char ch_id[USERINFOLEN];
        char ch_uri[USERINFOLEN];
        char ch_name[USERINFOLEN];
        char ch_passwd[USERINFOLEN];
        char ch_realm[USERINFOLEN];
        char ch_scheme[USERINFOLEN];

        pj_str_t uid = QstrToPstr("sip:"+this->username+"@"+this->domain,ch_id);
        pj_str_t reg_uri = QstrToPstr("sip:"+this->domain,ch_uri);
        pj_str_t username = QstrToPstr(this->username,ch_name);
        pj_str_t password = QstrToPstr(this->password,ch_passwd);
        pj_str_t realm = QstrToPstr(QString("*"),ch_realm);
        pj_str_t scheme = QstrToPstr(QString("digest"),ch_scheme);

    //    status = pjsua_verify_url(uid.ptr);


        {
            pjsua_acc_config cfg;
            pjsua_acc_config_default(&cfg);
            cfg.id = uid;
            cfg.reg_uri = reg_uri;
            cfg.cred_count = 1;
            cfg.cred_info[0].realm = realm;
            cfg.cred_info[0].scheme = scheme;
            cfg.cred_info[0].username = username;
            cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
            cfg.cred_info[0].data = password;
            status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
            if (status != PJ_SUCCESS) {
                QMessageBox::warning(NULL, "warning", "check username or password", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

                error_exit("Error adding account", status);
            } else {
                qDebug() << endl << "login success";
                ui->registerButton->setText("logout");
                SipOn = true;
            }
        }

    } else if (ui->registerButton->text() == "logout") {
        status = pjsua_acc_set_registration(acc_id,0);
        if (status == PJ_SUCCESS) {
            ui->registerButton->setText("register");
        } else {
            error("logout error",status);
        }

    }
}

void Siptel::on_callButton_clicked()
{
    if (!SipOn) {
        QMessageBox::warning( this, tr(USER_AGENT),
                tr("You are offline! Please REGISTER first!"),
                QMessageBox::Ok);
        return;
    }

    if (ui->callButton->text() == "call buddy") {

        QList<QListWidgetItem *> list = ui->buddyList->selectedItems();
        if (list.empty()) {
            return;
        }
        QListWidgetItem *curItem = ui->buddyList->currentItem();
        if (!curItem) {
            return;
        }

        activeCallsMutex.lock();
        if (!activeCalls.empty()) {
            activeCallsMutex.unlock();
            return;
        }

        pjsua_msg_data msg_data;
        pjsua_msg_data_init(&msg_data);
        int call_id;
        QString uri = curItem->data(Qt::UserRole).toString();
        char ch_uri[STRLEN];
        pj_str_t pj_uri = QstrToPstr(uri,ch_uri,STRLEN);
        pj_status_t status = pjsua_call_make_call(acc_id, &pj_uri, 0, 0, &msg_data, &call_id);

        if (status != PJ_SUCCESS) {
            error("Error calling buddy", status);
        } else {
            qDebug() << "call success";
            activeCalls << call_id;
            ui->callButton->setText("hang up");
        }
        activeCallsMutex.unlock();
    } else if (ui->callButton->text() == "answer call") {
        activeCallsMutex.lock();
        if (activeCalls.empty()) {
            ui->callButton->setText("call buddy");
            activeCallsMutex.unlock();
            return;
        }
        pjsua_call_answer(activeCalls.at(0),200,0,0);
        ui->callButton->setText("hang up");
        activeCallsMutex.unlock();
    } else if(ui->callButton->text() == "hang up") {
        activeCallsMutex.lock();
        ui->holdButton->setText("hold");
        // onHold = false;
        if (activeCalls.empty()) {
            ui->callButton->setText("call buddy");
            activeCallsMutex.unlock();
            return;
        }
        pj_status_t status;
        status = pjsua_call_hangup(activeCalls.at(0),0,0,0);
        if (status == PJ_SUCCESS) {
            activeCalls.removeAt(0);

        }
        activeCallsMutex.unlock();
    }
}
