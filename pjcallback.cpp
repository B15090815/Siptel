#include "pjcallback.h"
#include "appinfo.h"

void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
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

void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
 pjsua_call_info ci;

 PJ_UNUSED_ARG(e);

 pjsua_call_get_info(call_id, &ci);
 PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
 (int)ci.state_text.slen,
 ci.state_text.ptr));
}

/* Callback called by the library when call's media state has changed */
void on_call_media_state(pjsua_call_id call_id)
{
 pjsua_call_info ci;

 pjsua_call_get_info(call_id, &ci);

 if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
 // When media is active, connect call to sound device.
     pjsua_conf_connect(ci.conf_slot, 0);
     pjsua_conf_connect(0, ci.conf_slot);
 }
}


void error_exit(const char *title, pj_status_t status)
{
  pjsua_perror(THIS_FILE, title, status);
  pjsua_destroy();
//  exit(1);
}

void error(const char *title, pj_status_t status) {
    pjsua_perror(LOGPREFIX, title, status);
}
