#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iup/iup.h>
#include <iup/iup_config.h>
#include <errno.h>
#include <sqlite3.h>

#include "utf8.h"
#include "doerror.h"
#include "types.h"
#include "dbaccess.h"
#include "windowlist.h"

#include "textgotobox.h"

int tgb_goto_ok_action_cb(Ihandle *bt_ok)
{
  long line_count = (long) IupGetInt(bt_ok, "TEXT_LINECOUNT");
  Ihandle *txt = IupGetDialogChild(bt_ok, "LINE_TEXT");
  long line = (long) IupGetInt(txt, "VALUE");
  if (line < 1 || line >= line_count)
  {
    IupMessage("Error", "Invalid Line Number!");
    return IUP_DEFAULT;
  }
  
  IupSetAttribute(IupGetDialog(bt_ok), "STATUS", "1");
  return IUP_CLOSE;
}

int tgb_goto_cancel_action_cb(Ihandle *bt_cancel)
{
  IupSetAttribute(IupGetDialog(bt_cancel), "STATUS", "0");
  return IUP_CLOSE;
}

int tgb_do_goto(Ihandle *parentdlg, char *prevwindowlist, int line_count)
{
  Ihandle *dlg, *box, *bt_ok, *bt_cancel, *txt, *lbl;
  int ans = 0;
  
  lbl = IupLabel(NULL);
  IupSetfAttribute(lbl, "TITLE", "Line Number [1-%d]:", line_count);
  
  txt = IupText(NULL);
  IupSetAttribute(txt, "MASK", IUP_MASK_UINT);
  IupSetAttribute(txt, "NAME", "LINE_TEXT");
  IupSetAttribute(txt, "VISIBLECOLUMNS", "20");
  
  bt_ok = IupButton("&OK", NULL);
  IupSetInt(bt_ok, "TEXT_LINECOUNT", line_count);
  IupSetAttribute(bt_ok, "PADDING", "10x2");
  IupSetCallback(bt_ok, "ACTION", (Icallback) tgb_goto_ok_action_cb);
  
  bt_cancel = IupButton("&Cancel",NULL);
  IupSetAttribute(bt_cancel, "PADDING", "10x2");
  IupSetCallback(bt_cancel, "ACTION", (Icallback) tgb_goto_cancel_action_cb);
  
  box = IupVbox(
    lbl,
    txt,
    IupSetAttributes(IupHbox(
      IupFill(),
      bt_ok,
      bt_cancel,
      NULL), "NORMALIZESIZE=HORIZONTAL"),
    NULL);
  IupSetAttribute(box, "MARGIN", "10x10");
  IupSetAttribute(box, "GAP", "5");
  
  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "Go To Line");
  IupSetAttribute(dlg, "DIALOGFRAME", "YES");
  IupSetStrAttribute(dlg, "PREVIOUS_WINDOW_LIST", prevwindowlist);
  IupSetAttributeHandle(dlg, "DEFAULTENTER", bt_ok);
  IupSetAttributeHandle(dlg, "DEFAULTESC", bt_cancel);
  IupSetAttributeHandle(dlg, "PARENTDIALOG", parentdlg);
  
  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
  
  if (IupGetInt(dlg, "STATUS") == 1)
  {
    int line = IupGetInt(txt, "VALUE");
    ans = line;
  }
  
  IupDestroy(dlg);
  
  return ans;
}

