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
int newperson_cancelbtn_action_cb(Ihandle *ih);
int newperson_helpbtn_action_cb(Ihandle *ih);

sqlite3_int64 addorupdaterecord(Ihandle *dlg, sqlite3_int64 recordid);

sqlite3_int64 donewperson(Ihandle *parentdlg, Ihandle *config, const char *prevwindows);

#endif

/*
  Fields' Attributes/Controls
  ------------------------------
  Att: BIO_TEXT
  Con: ETHNIC_TXT, ETHNIC_UNKNOWN_TGL
  Con: CSEX_MALE_TGL, CSEX_FEMALE_TGL, CSEX_UNKNOWN_TGL
  Con: GENDER_TXT, GENDER_UNKNOWN_TGL
  DB (Names)
  Att: NOTES_TEXT
  (BRel)
  (BDate)
  Con: BIRTH_PLACE_TXT, BIRTH_PLACE_UNKNOWN_TGL
  (BAddr), Con: BIRTH_ADDRESS_NA_TGL
  Con: DEATH_ALIVE_TGL
  (DDate)
  Con: DEATH_PLACE_TXT, DEATH_PLACE_UNKNOWN_TGL
  (DAddr), Con: DEATH_ADDRESS_NA_TGL
  (DAge)
  (Source)
*/