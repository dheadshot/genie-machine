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

#include "textedit.h"


/* ------------- Utilities ------------- */

void tedit_toggle_bar_visibility(Ihandle *item, Ihandle *ih)
{
  if (IupGetInt(item, "VALUE"))
  {
    IupSetAttribute(ih, "FLOATING", "YES");
    IupSetAttribute(ih, "VISIBLE", "NO");
    IupSetAttribute(item, "VALUE", "OFF");
  }
  else
  {
    IupSetAttribute(ih, "FLOATING", "NO");
    IupSetAttribute(ih, "VISIBLE", "YES");
    IupSetAttribute(item, "VALUE", "ON");
  }
  
  IupRefresh(ih); /* Refresh the Dialogue Layout */
}


/* ------------- CallBacks ------------- */

int tedit_cancelbtn_action_cb(Ihandle *cancelbtn)
{
  Ihandle *dlg = IupGetDialog(cancelbtn);
  Ihandle *txt = IupGetDialogChild(okbtn, "MAIN_TEXT");
  int dirty = IupGetInt(txt, "DIRTY");
  
  /* "Are you sure you wish to Cancel?" */
  if (dirty && IupAlarm("Warning", 
      "Are you sure you want to close the text editor?  You will lose all changes you have made to the contents!", "Yes", "No", NULL) != 1)
  {
    return IUP_IGNORE;
  }
  
  IupSetAttribute(dlg, "STATUS", "0");
  return IUP_CLOSE;
}

int tedit_okbtn_action_cb(Ihandle *okbtn)
{
  Ihandle *dlg = IupGetDialog(okbtn);
  Ihandle *txt = IupGetDialogChild(okbtn, "MAIN_TEXT");
  
  char *returntext = IupGetAttribute(txt, "VALUE");
  
  IupSetStrAttribute(dlg, "RETURN_TEXT", returntext);
  IupSetAttribute(dlg, "STATUS", 1);
  return IUP_CLOSE;
}

int tedit_maintext_valuechanged_cb(Ihandle *ih)
{
  Ihandle *txt = IupGetDialogChild(ih, "MAIN_TEXT");
  IupSetAttribute(txt, "DIRTY", "YES");
  return IUP_DEFAULT;
}

int tedit_edit_menu_open_cb(Ihandle *ih)
{
  Ihandle *txt = IupGetDialogChild(ih, "MAIN_TEXT");
  Ihandle *clipboard = IupClipboard();
  Ihandle *item_paste = IupGetDialogChild(ih, "ITEM_PASTE");
  Ihandle *item_cut = IupGetDialogChild(ih, "ITEM_CUT");
  Ihandle *item_delete = IupGetDialogChild(ih, "ITEM_DELETE");
  Ihandle *item_copy = IupGetDialogChild(ih, "ITEM_COPY");
  
  if (!IupGetInt(clipboard, "TEXTAVAILABLE"))
  {
    IupSetAttribute(item_paste, "ACTIVE", "NO");
  }
  else
  {
    IupSetAttribute(item_paste, "ACTIVE", "YES");
  }
  
  if (!IupGetAttribute(txt, "SELECTEDTEXT"))
  {
    IupSetAttribute(item_cut, "ACTIVE", "NO");
    IupSetAttribute(item_copy, "ACTIVE", "NO");
    IupSetAttribute(item_delete, "ACTIVE", "NO");
  }
  else
  {
    IupSetAttribute(item_cut, "ACTIVE", "YES");
    IupSetAttribute(item_copy, "ACTIVE", "YES");
    IupSetAttribute(item_delete, "ACTIVE", "YES");
  }
  
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

int tedit_maintext_caret_cb(Ihandle *ih, int lin, int col)
{
  Ihandle *lbl_statusbar = IupGetDialogChild(ih, "STATUSBAR");
  IupSetfAttribute(lbl_statusbar, "TITLE", "Line: %d; Column: %d.", lin, col);
  return IUP_DEFAULT;
}

int tedit_item_revert_action_cb(Ihandle *item_revert)
{
  Ihandle *txt = IupGetDialogChild(ih, "MAIN_TEXT");
  int dirty = IupGetInt(txt, "DIRTY");
  
  /* "Are you sure you wish to Revert?" */
  if (dirty && IupAlarm("Warning", 
      "Are you sure you want to revert all changes?  You will lose all changes you have made to the contents!", "Yes", "No", NULL) == 1)
  {
    char *returntext = IupGetAttribute(dlg, "RETURN_TEXT");
    IupSetStrAttribute(txt, "VALUE", returntext);
    IupSetAttribute(txt, "DIRTY", "NO");
  }
  return IUP_DEFAULT;
}

int tedit_item_copy_action_cb(Ihandle *item_copy)
{
  Ihandle *clipboard = IupClipboard();
  Ihandle *txt = IupGetDialogChild(ih, "MAIN_TEXT");
  if (IupGetAttribute(txt, "SELECTEDTEXT"))
  {
    IupSetAttribute(clipboard, "TEXT", IupGetAttribute(txt, "SELECTEDTEXT"));
  }
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

int tedit_item_paste_action_cb(Ihandle *item_paste)
{
  Ihandle *txt = IupGetDialogChild(ih, "MAIN_TEXT");
  Ihandle *clipboard = IupClipboard();
  if (IupGetInt(clipboard, "TEXTAVAILABLE"))
  {
    IupSetAttribute(txt, "INSERT", IupGetAttribute(clipboard, "TEXT"));
    IupSetAttribute(txt, "DIRTY", "YES");
  }
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

int tedit_item_cut_action_cb(Ihandle *item_cut)
{
  Ihandle *clipboard = IupClipboard();
  Ihandle *txt = IupGetDialogChild(ih, "MAIN_TEXT");
  if (IupGetAttribute(txt, "SELECTEDTEXT"))
  {
    IupSetAttribute(clipboard, "TEXT", IupGetAttribute(txt, "SELECTEDTEXT"));
    IupSetAttribute(txt, "SELECTEDTEXT", "");
    IupSetAttribute(txt, "DIRTY", "YES");
  }
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

int tedit_item_delete_action_cb(Ihandle *item_delete)
{
  Ihandle *txt = IupGetDialogChild(ih, "MAIN_TEXT");
  if (IupGetAttribute(txt, "SELECTEDTEXT"))
  {
    IupSetAttribute(txt, "SELECTEDTEXT", "");
    IupSetAttribute(txt, "DIRTY", "YES");
  }
  return IUP_DEFAULT;
}

int tedit_item_select_all_action_cb(Ihandle *item_select_all)
{
  Ihandle *txt = IupGetDialogChild(ih, "MAIN_TEXT");
  IupSetFocus(txt);
  IupSetAttribute(txt, "SELECTION", "ALL");
  return IUP_DEFAULT;
}

int tedit_item_toolbar_action_cb(Ihandle *item_toolbar)
{
  Ihandle *toolbar = IupGetDialogChild(item_toolbar, "TEDIT_TOOLBAR");
  Ihandle *config = (Ihandle *) IupGetAttribute(IupGetDialog(item_toolbar), "CONFIG");
  
  toggle_bar_visibility(item_toolbar, toolbar);
  
  IupConfigSetVariableStr(config, "TextEditor", "Toolbar", IupGetAttribute(item_toolbar, "VALUE"));
  return IUP_DEFAULT;
}

int tedit_item_statusbar_action_cb(Ihandle *item_statusbar)
{
  Ihandle *statusbar = IupGetDialogChild(item_statusbar, "TEDIT_STATUSBAR");
  Ihandle *config = (Ihandle *) IupGetAttribute(IupGetDialog(item_statusbar), "CONFIG");
  
  toggle_bar_visibility(item_statusbar, statusbar);
  
  IupConfigSetVariableStr(config, "TextEditor", "Statusbar", IupGetAttribute(item_statusbar, "VALUE"));
  return IUP_DEFAULT;
}

int tedit_item_help_action_cb(Ihandle *ih)
{
  if (IupHelp("https://github.com/dheadshot/genie-machine") < 1)
  {
    IupMessage("Error Opening Help","Error opening help file!  Please set the web browser for your system in the main window, using \"Help>Set Browser...\".");
  }
  return IUP_DEFAULT;
}
