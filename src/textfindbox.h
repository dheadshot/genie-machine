#ifndef __TEXTFINDBOX_H__
#define __TEXTFINDBOX_H__ 1

#include <iup/iup.h>
#include <iup/iup_config.h>

int tfb_find_next_action_cb(Ihandle *bt_next);
int tfb_find_close_action_cb(Ihandle *bt_close);
Ihandle *tfb_create(Ihandle *config, Ihandle *maintext, Ihandle *parentdlg);
void tfb_create_or_show(Ihandle *parentdlg, Ihandle *maintext, Ihandle *config, char *prevwindowlist);
void tfb_free(Ihandle *dlg);

#endif