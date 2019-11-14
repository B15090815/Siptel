#ifndef UTIL_H
#define UTIL_H
#include <pjsua-lib/pjsua.h>
#include <QString>

#define USERINFOLEN 50

pj_str_t QstrToPstr(QString src,char dest[]);

#endif // UTIL_H
