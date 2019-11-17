#ifndef UTIL_H
#define UTIL_H
#include <pjsua-lib/pjsua.h>
#include <QString>
#define USERINFOLEN 50
#define STRLEN 100

pj_str_t QstrToPstr(QString src,char dest[]);
pj_str_t QstrToPstr(QString src,char dest[],int size);
pj_str_t QstrToPstr(QString src);
#endif // UTIL_H
