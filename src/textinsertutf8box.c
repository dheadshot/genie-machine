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

#include "textinsertutf8box.h"


int tiutf8b_insertutf8_ok_action_cb(Ihandle *ih)
{
  Ihandle *txt = IupGetDialogChild(ih, "UTF8OFFSET_TXT");
  char *str = IupGetAttribute(txt, "VALUE");
  if (!str || !str[0]) IupMessage("Error","You must enter a value!");
  else
  {
    IupSetAttribute(IupGetDialog(ih), "STATUS", "1");
    return IUP_CLOSE;
  }
  return IUP_DEFAULT;
}

int tiutf8b_insertutf8_cancel_action_cb(Ihandle *ih)
{
  IupSetAttribute(IupGetDialog(ih), "STATUS", "0");
  return IUP_CLOSE;
}

char *tiutf8b_do_insertutf8(Ihandle *parentdlg, char *prevwindowlist)
{
  Ihandle *utf8dlg, *vbox, *hbox, *okbtn, *cancelbtn, *txt, *lbl;
  
  char *itxt = NULL;
  
  lbl = IupLabel(NULL);
  IupSetAttribute(lbl, "TITLE", "Enter Unicode Offset in Hexadecimal:");
  
  txt = IupText(NULL);
  IupSetAttribute(txt, "MASK", "([uU]/x2B)?[0-1]?[0-9a-fA-F]?[0-9a-fA-F]?[0-9a-fA-F]?[0-9a-fA-F]?[0-9a-fA-F]");
  IupSetAttribute(txt, "NAME", "UTF8OFFSET_TXT");
  IupSetAttribute(txt, "VISIBLECOLUMNS", "8");
  
  okbtn = IupButton("&OK", NULL);
  IupSetAttribute(okbtn, "PADDING", "10x2");
  IupSetCallback(okbtn, "ACTION", (Icallback) tiutf8b_insertutf8_ok_action_cb);
  
  cancelbtn = IupButton("&Cancel", NULL);
  IupSetAttribute(cancelbtn, "PADDING", "10x2");
  IupSetCallback(cancelbtn, "ACTION", (Icallback) tiutf8b_insertutf8_cancel_action_cb);
  
  hbox = IupHbox(IupFill(), okbtn, cancelbtn, NULL);
  
  vbox = IupVbox(lbl, txt, IupSetAttributes(hbox, "NORMALIZESIZE=HORIZONTAL"), NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "5");
  
  utf8dlg = IupDialog(vbox);
  IupSetAttribute(utf8dlg, "TITLE", "Insert Unicode Codepoint");
  IupSetAttribute(utf8dlg, "DIALOGFRAME", "YES");
  IupSetStrAttribute(utf8dlg, "PREVIOUS_WINDOW_LIST", prevwindowlist);
  IupSetAttributeHandle(utf8dlg, "DEFAULTENTER", okbtn);
  IupSetAttributeHandle(utf8dlg, "DEFAULTESC", cancelbtn);
  IupSetAttributeHandle(utf8dlg, "PARENTDIALOG", parentdlg);
  
  IupPopup(utf8dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
  
  if (IupGetInt(utf8dlg, "STATUS") == 1)
  {
    char *ucp = IupGetAttribute(txt, "VALUE");
    char *ucpo = ucp;
    unsigned long ucpv = 0;
    if (toupper(ucp[0]) == 'U') ucpo+=sizeof(char);
    if (ucpo[0] == '+') ucpo+=sizeof(char);
    sscanf(ucpo,"%6lx", &ucpv);
    if (ucpv > 0)
    {
      itxt = utf8char(ucpv);
      if (itxt == NULL) show_error("Invalid Unicode Codepoint!", 1, "UTF-8 Error", parentdlg);
    }
    else
    {
      show_error("Cannot insert a NUL character!", 1, "UTF-8 Error", parentdlg);
    }
  }
  IupDestroy(utf8dlg);
  
  return itxt;
}
