#ifndef __NEWPERSON_H__
#define __NEWPERSON_H__ 1

#include <iup/iup.h>
#include <iup/iup_config.h>
#include <sqlite3.h>

int newperson_bioeditbtn_action_cb(Ihandle *bioeditbtn);
int newperson_ethnicunktgl_action_cb(Ihandle *ethnicunktgl, int state);
int newperson_genunktgl_action_cb(Ihandle *genunktgl, int state);
int newperson_noteeditbtn_action_cb(Ihandle *noteeditbtn);
int newperson_bplaceunktgl_action_cb(Ihandle *bplaceunktgl, int state);
int newperson_baddrnatgl_action_cb(Ihandle *baddrnatgl, int state);
int newperson_dalivetgl_action_cb(Ihandle *dalivetgl, int state);
int newperson_dplaceunktgl_action_cb(Ihandle *dplaceunktgl, int state);
int newperson_daddrnatgl_action_cb(Ihandle *daddrnatgl, int state);
int newperson_helpbtn_action_cb(Ihandle *ih);

sqlite3_int64 donewperson(Ihandle *parentdlg, Ihandle *config, const char *prevwindows);

#endif
