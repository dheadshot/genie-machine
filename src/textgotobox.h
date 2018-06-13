#ifndef __TEXTGOTOBOX_H__
#define __TEXTGOTOBOX_H__ 1

#include <iup/iup.h>
#include <iup/iup_config.h>

int tgb_goto_ok_action_cb(Ihandle *bt_ok);
int tgb_goto_cancel_action_cb(Ihandle *bt_cancel);
int tgb_do_goto(Ihandle *parentdlg, char *prevwindowlist, int line_count);

#endif