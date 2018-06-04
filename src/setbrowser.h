#ifdef __SETBROWSER_H__

#define __SETBROWSER_H__

#include <iup/iup.h>

int setbrowser_test_action_cb(Ihandle *ih);
int setbrowser_txt_valuechanged_cb(Ihandle *ih);
int setbrowser_ok_action_cb(Ihandle *ih);
int setbrowser_cancel_action_cb(Ihandle *ih);
int dosetbrowser(Ihandle *parentdlg, *Ihandle config);


#endif