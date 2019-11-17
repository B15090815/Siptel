#include "callfunc.h"
#include <QList>
#include <QMutex>
#define THIS_FILE "CallFunc"

extern QList<int> activeCalls;
extern QMutex activeCallsMutex;
CallFunc *globalPjCallback;

CallFunc::CallFunc()
{
    globalPjCallback = this;
}

CallFunc::~CallFunc()
{

}

void CallFunc::logger_cb(int level, const char *data, int len) {
    PJ_UNUSED_ARG(level);
    PJ_UNUSED_ARG(len);
    /* optional dump to stdout */
    /* emit signal with log message */
    /* paramter will be converted to QString which makes a deep copy */
    emit new_log_message(data);
}

void CallFunc::logger_cb_wrapper(int level, const char *data, int len) {
    if (globalPjCallback) {
        CallFunc *myCb = (CallFunc*) globalPjCallback;
        myCb->logger_cb(level, data, len);
    }
}

void CallFunc::on_pager(pjsua_call_id call_id, const pj_str_t *from,
        const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
        const pj_str_t *text)
{
    /* Note: call index may be -1 */
    PJ_UNUSED_ARG(call_id);
    PJ_UNUSED_ARG(to);
    PJ_UNUSED_ARG(contact);
    PJ_UNUSED_ARG(mime_type);

    emit new_im(QString::fromLatin1(from->ptr,from->slen),
            QString::fromLatin1(text->ptr,text->slen));
}

void CallFunc::on_pager_wrapper(pjsua_call_id call_id, const pj_str_t *from,
        const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
        const pj_str_t *text)
{
    /* call the non-static member */
    if (globalPjCallback) {
        CallFunc *myCb = (CallFunc*) globalPjCallback;
        myCb->on_pager(call_id, from, to, contact, mime_type, text);
    }
}

void CallFunc::on_call_state(pjsua_call_id call_id, pjsip_event *e) {
    PJ_UNUSED_ARG(e);
    if (activeCalls.empty()) {
        PJ_LOG(3,(THIS_FILE, "Call %d not found as callList is empty; new incoming call? ... ignoring", call_id));
        return;
    }
    if (!activeCalls.contains(call_id)) {
        PJ_LOG(3,(THIS_FILE, "Call %d not found in callList; new incoming call? ... ignoring", call_id));
        return;
    }

    pj_status_t status;
    pjsua_call_info ci;
    status = pjsua_call_get_info(call_id, &ci);
    if (status != PJ_SUCCESS) {
        PJ_LOG(3,(THIS_FILE, "ERROR retrieveing info for Call %d ... ignoring", call_id));
        return;
    }
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
            (int)ci.state_text.slen, ci.state_text.ptr));

    QString state_text = QString::fromLatin1(ci.state_text.ptr,(int)ci.state_text.slen);
    emit setCallState(state_text);

    switch(ci.state) {
    case PJSIP_INV_STATE_DISCONNECTED:
        activeCalls.removeAt(activeCalls.indexOf(call_id));
        emit setCallButtonText("call buddy");
        break;
    default:
        ;
    }
}

void CallFunc::on_call_state_wrapper(pjsua_call_id call_id, pjsip_event *e) {
    /* call the non-static member */
    if (globalPjCallback) {
        CallFunc *myCb = (CallFunc*) globalPjCallback;
        myCb->on_call_state(call_id, e);
    }
}

void CallFunc::on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    activeCallsMutex.lock();
    if (!activeCalls.empty()) {
        PJ_LOG(3,(THIS_FILE, "new incoming Call %d, but we already have a call ... reject", call_id));
        activeCallsMutex.unlock();
        pjsua_call_hangup(call_id, 486, NULL, NULL);
        return;
    }

    pj_status_t status;
    pjsua_call_info ci;
    status = pjsua_call_get_info(call_id, &ci);
    if (status != PJ_SUCCESS) {
        PJ_LOG(3,(THIS_FILE, "ERROR retrieveing info for Call %d ... ignoring", call_id));
        activeCallsMutex.unlock();
        return;
    }
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
            (int)ci.state_text.slen, ci.state_text.ptr));

    QString state_text = QString::fromLatin1(ci.state_text.ptr,(int)ci.state_text.slen);
    emit setCallState(state_text);

    emit setCallState(state_text);
    activeCalls << call_id;
    emit setCallButtonText("answer call");
    activeCallsMutex.unlock();
}

void CallFunc::on_incoming_call_wrapper(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
    /* call the non-static member */
    if (globalPjCallback) {
        CallFunc *myCb = (CallFunc*) globalPjCallback;
        myCb->on_incoming_call(acc_id, call_id, rdata);
    }
}

void CallFunc::on_call_media_state(pjsua_call_id call_id) {
    pjsua_call_info ci;

    pjsua_call_get_info(call_id, &ci);
    switch (ci.media_status) {
    case PJSUA_CALL_MEDIA_NONE:
        PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
                "PJSUA_CALL_MEDIA_NONE: Call currently has no media", call_id));
        break;
    case PJSUA_CALL_MEDIA_ACTIVE:
        PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
                "PJSUA_CALL_MEDIA_ACTIVE: The media is active", call_id));
        // When media is active, connect call to sound device.
        pjsua_conf_connect(ci.conf_slot, 0);
        pjsua_conf_connect(0, ci.conf_slot);
        break;
    case PJSUA_CALL_MEDIA_LOCAL_HOLD:
        PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
                "PJSUA_CALL_MEDIA_LOCAL_HOLD: The media is currently put on hold by local endpoint", call_id));
        break;
    case PJSUA_CALL_MEDIA_REMOTE_HOLD:
        PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
                "PJSUA_CALL_MEDIA_REMOTE_HOLD: The media is currently put on hold by remote endpoint", call_id));
        break;
    case PJSUA_CALL_MEDIA_ERROR:
        PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
                "PJSUA_CALL_MEDIA_ERROR: The media has reported error (e.g. ICE negotiation)", call_id));
        break;
    }
}

void CallFunc::on_call_media_state_wrapper(pjsua_call_id call_id) {
    /* call the non-static member */
    if (globalPjCallback) {
        CallFunc *myCb = (CallFunc*) globalPjCallback;
        myCb->on_call_media_state(call_id);
    }
}


void CallFunc::on_buddy_state(pjsua_buddy_id buddy_id) {
    emit new_log_message("on_buddy_state called for buddy_id " + QString::number(buddy_id));
    emit buddy_state(buddy_id);
}

void CallFunc::on_buddy_state_wrapper(pjsua_buddy_id buddy_id) {
    /* call the non-static member */
    if (globalPjCallback) {
        CallFunc *myCb = (CallFunc*) globalPjCallback;
        myCb->on_buddy_state(buddy_id);
    }
}

void CallFunc::on_reg_state(pjsua_acc_id acc_id) {
    emit reg_state_signal(acc_id);
}

void CallFunc::on_reg_state_wrapper(pjsua_acc_id acc_id) {
    /* call the non-static member */
    if (globalPjCallback) {
        CallFunc *myCb = (CallFunc*) globalPjCallback;
        myCb->on_reg_state(acc_id);
    }
}
