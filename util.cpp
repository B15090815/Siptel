#include "util.h"

pj_str_t QstrToPstr(QString src,char dest[])
{
    memset(dest,0,USERINFOLEN);
    memcpy(dest,src.toLatin1().data(),src.size());
    return pj_str(dest);
}
