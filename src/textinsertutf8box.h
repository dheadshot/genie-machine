#ifndef __TEXTINSERTUTF8BOX_H__
#define __TEXTINSERTUTF8BOX_H__ 1

#include <iup/iup.h>
#include <iup/iup_config.h>

int tiutf8b_insertutf8_ok_action_cb(Ihandle *ih);
int tiutf8b_insertutf8_cancel_action_cb(Ihandle *ih);
char *tiutf8b_do_insertutf8(Ihandle *parentdlg, char *prevwindowlist);

#endif