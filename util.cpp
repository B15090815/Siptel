#include "util.h"
//char QPstr[STRLEN];
pj_str_t QstrToPstr(QString src,char dest[])
{
    memset(dest,0,USERINFOLEN);
    memcpy(dest,src.toLatin1().data(),src.size());
    return pj_str(dest);
}

pj_str_t QstrToPstr(QString src,char dest[],int size)
{
    memset(dest,0,size);
    memcpy(dest,src.toLatin1().data(),src.size());
    return pj_str(dest);
}

//pj_str_t QstrToPstr(QString src)
//{

//    memset(QPstr,0,STRLEN);
//    memcpy(QPstr,src.toLatin1().data(),src.size());
//    return pj_str(QPstr);
//}
