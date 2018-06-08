#ifndef __NEWPERSON_H__
#define __NEWPERSON_H__ 1

#include <iup/iup.h>
#include <iup/iup_config.h>
#include <sqlite3.h>

sqlite3_int64 donewperson(Ihandle *parentdlg, Ihandle *config, const char *prevwindows);

#endif