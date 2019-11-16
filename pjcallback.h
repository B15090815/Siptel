#ifndef PJCALLBACK_H
#define PJCALLBACK_H
#include <pjsua-lib/pjsua.h>

void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,pjsip_rx_data *rdata);
void on_call_state(pjsua_call_id call_id, pjsip_event *e);
void on_call_media_state(pjsua_call_id call_id);
void error_exit(const char *title, pj_status_t status);
void error(const char *title, pj_status_t status);
#endif // PJCALLBACK_H
