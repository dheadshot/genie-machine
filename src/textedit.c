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

#include "textfindbox.h"
#include "textgotobox.h"
#include "textinsertutf8box.h"

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
  Ihandle *txt = IupGetDialogChild(dlg, "MAIN_TEXT");
  int dirty = IupGetInt(txt, "DIRTY");
  
  /* "Are you sure you wish to Cancel?" */
  if (dirty && IupAlarm("Warning", 
      "Are you sure you want to close the text editor?  You will lose all changes you have made to the contents!", "Yes", "No", NULL) != 1)
  {
    return IUP_IGNORE;
  }
  
  Ihandle *finddlg = (Ihandle *) IupGetAttribute(dlg, "FIND_DIALOG");
  if (finddlg)
  {
    tfb_free(finddlg);
    IupSetAttribute(dlg, "FIND_DIALOG", NULL);
  }
  
  IupSetAttribute(dlg, "STATUS", "0");
  IupSetAttribute(dlg, "CONFIG", NULL);
  return IUP_CLOSE;
}

int tedit_okbtn_action_cb(Ihandle *okbtn)
{
  Ihandle *dlg = IupGetDialog(okbtn);
  Ihandle *txt = IupGetDialogChild(dlg, "MAIN_TEXT");
  
  char *returntext = IupGetAttribute(txt, "VALUE");
  
  Ihandle *finddlg = (Ihandle *) IupGetAttribute(dlg, "FIND_DIALOG");
  if (finddlg)
  {
    tfb_free(finddlg);
    IupSetAttribute(dlg, "FIND_DIALOG", NULL);
  }
  IupSetAttribute(dlg, "CONFIG", NULL);
  
  if (returntext) IupSetStrAttribute(dlg, "RETURN_TEXT", returntext);
  else IupSetStrAttribute(dlg, "RETURN_TEXT", "");
  IupSetAttribute(dlg, "STATUS", "1");
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
  Ihandle *dlg = IupGetDialog(item_revert);
  Ihandle *txt = IupGetDialogChild(dlg, "MAIN_TEXT");
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
  Ihandle *txt = IupGetDialogChild(item_copy, "MAIN_TEXT");
  if (IupGetAttribute(txt, "SELECTEDTEXT"))
  {
    IupSetAttribute(clipboard, "TEXT", IupGetAttribute(txt, "SELECTEDTEXT"));
  }
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

int tedit_item_paste_action_cb(Ihandle *item_paste)
{
  Ihandle *txt = IupGetDialogChild(item_paste, "MAIN_TEXT");
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
  Ihandle *txt = IupGetDialogChild(item_cut, "MAIN_TEXT");
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
  Ihandle *txt = IupGetDialogChild(item_delete, "MAIN_TEXT");
  if (IupGetAttribute(txt, "SELECTEDTEXT"))
  {
    IupSetAttribute(txt, "SELECTEDTEXT", "");
    IupSetAttribute(txt, "DIRTY", "YES");
  }
  return IUP_DEFAULT;
}

int tedit_item_select_all_action_cb(Ihandle *item_select_all)
{
  Ihandle *txt = IupGetDialogChild(item_select_all, "MAIN_TEXT");
  IupSetFocus(txt);
  IupSetAttribute(txt, "SELECTION", "ALL");
  return IUP_DEFAULT;
}

int tedit_item_toolbar_action_cb(Ihandle *item_toolbar)
{
  Ihandle *toolbar = IupGetDialogChild(item_toolbar, "TEDIT_TOOLBAR");
  Ihandle *config = (Ihandle *) IupGetAttribute(IupGetDialog(item_toolbar), "CONFIG");
  
  tedit_toggle_bar_visibility(item_toolbar, toolbar);
  
  IupConfigSetVariableStr(config, "TextEditor", "Toolbar", IupGetAttribute(item_toolbar, "VALUE"));
  return IUP_DEFAULT;
}

int tedit_item_statusbar_action_cb(Ihandle *item_statusbar)
{
  Ihandle *statusbar = IupGetDialogChild(item_statusbar, "TEDIT_STATUSBAR");
  Ihandle *config = (Ihandle *) IupGetAttribute(IupGetDialog(item_statusbar), "CONFIG");
  
  tedit_toggle_bar_visibility(item_statusbar, statusbar);
  
  IupConfigSetVariableStr(config, "TextEditor", "Statusbar", IupGetAttribute(item_statusbar, "VALUE"));
  return IUP_DEFAULT;
}

int tedit_item_help_action_cb(Ihandle *ih)
{
  if (IupHelp("https://github.com/dheadshot/genie-machine") < 1) //TODO: Change this to the correct HELP link!
  {
    /*IupMessage("Error Opening Help","Error opening help file!  Please set the web browser for your system in the main window, using \"Help>Set Browser...\".");*/
    show_error("Error opening help file!  Please set the web browser for your system in the main window, using \"Help>Set Browser...\".",1,"Error Opening Help",IupGetDialog(ih));
  }
  return IUP_DEFAULT;
}

int tedit_item_find_action_cb(Ihandle *item_find)
{
  Ihandle *dlg = IupGetDialog(item_find);
  Ihandle *config = (Ihandle *) IupGetAttribute(dlg, "CONFIG");
  Ihandle *mtext = IupGetDialogChild(dlg, "MAIN_TEXT");
  char *windowlist = IupGetAttribute(dlg, "WINDOW_LIST");
  
  tfb_create_or_show(dlg, mtext, config, windowlist);
  
  return IUP_DEFAULT;
}

int tedit_item_goto_action_cb(Ihandle *item_goto)
{
  Ihandle *dlg = IupGetDialog(item_goto);
  Ihandle *maintext = IupGetDialogChild(dlg, "MAIN_TEXT");
  char *windowlist = IupGetAttribute(dlg, "WINDOW_LIST");
  int line_count = IupGetInt(maintext, "LINECOUNT");
  
  int gotoline = tgb_do_goto(dlg, windowlist, line_count);
  
  if (gotoline)
  {
    int pos;
    IupTextConvertLinColToPos(maintext, gotoline, 0, &pos);
    IupSetInt(maintext, "CARETPOS", pos);
    IupSetInt(maintext, "SCROLLTOPOS", pos);
    IupSetFocus(maintext);
    tedit_maintext_caret_cb(maintext, gotoline, 1);
  }
  
  return IUP_DEFAULT;
}

int tedit_insertutf8_action_cb(Ihandle *item_insertutf8)
{
  Ihandle *dlg = IupGetDialog(item_insertutf8);
  Ihandle *maintext = IupGetDialogChild(dlg, "MAIN_TEXT");
  char *windowlist = IupGetAttribute(dlg, "WINDOW_LIST");
  
  char *itxt = tiutf8b_do_insertutf8(dlg, windowlist);
  
  if (itxt)
  {
    IupSetStrAttribute(maintext, "INSERT", itxt);
    IupSetFocus(maintext);
    free(itxt); //Hopefully we can do this!
  }
  
  return IUP_DEFAULT;
}

/* ----------- Main Functions ----------- */

Ihandle *tedit_makedlg(Ihandle *config, const char *caption)
{
  Ihandle *dlg, *vbox;
  Ihandle *contents_menu, *item_cancel, *item_ok, *item_revert;
  Ihandle *edit_menu, *item_find, *item_goto, *item_copy, *item_paste, 
          *item_cut, *item_delete, *item_select_all;
  Ihandle *insert_menu, *item_codepoint;
  Ihandle *view_menu, *item_toolbar, *item_statusbar;
  Ihandle *help_menu, *item_help;
  Ihandle *sub_menu_contents, *sub_menu_edit, *sub_menu_format, *sub_menu_help, 
          *sub_menu_insert, *sub_menu_view, *menu;
  Ihandle *lbl_statusbar, *toolbar_hb, *maintext, *btns_hb;
  Ihandle *cancelbtn, *okbtn, *helpbtn;
  Ihandle *btn_find, *btn_goto, *btn_copy, *btn_cut, *btn_paste,
          *btn_delete;
  
  maintext = IupText(NULL);
  IupSetAttribute(maintext, "NAME", "MAIN_TEXT");
  IupSetAttribute(maintext, "MULTILINE", "YES");
  IupSetAttribute(maintext, "EXPAND", "YES");
  IupSetAttribute(maintext, "DIRTY", "NO");
  
  const char *font = IupConfigGetVariableStr(config, "TextEditor", "Font");
  if (font)
  {
    IupSetStrAttribute(maintext, "FONT", font);
  }
  
  lbl_statusbar = IupLabel("Line: 1; Column: 1.");
  IupSetAttribute(lbl_statusbar, "NAME", "STATUSBAR");
  IupSetAttribute(lbl_statusbar, "EXPAND", "HORIZONTAL");
  IupSetAttribute(lbl_statusbar, "PADDING", "10x5");
  
  item_cancel = IupItem("&Cancel\tEsc",NULL);
  IupSetAttribute(item_cancel, "IMAGE", "IUP_ActionCancel");
  
  item_ok = IupItem("&OK\tCtrl+Enter", NULL);
  IupSetAttribute(item_ok, "IMAGE", "IUP_ActionOK");
  
  item_revert = IupItem("&Revert", NULL);
  IupSetAttribute(item_revert, "NAME", "ITEM_REVERT");
  IupSetAttribute(item_revert, "IMAGE", "IUP_MediaGoToBegin");
  
  item_find = IupItem("&Find...\tCtrl+F",NULL);
  IupSetAttribute(item_find, "IMAGE", "IUP_EditFind");
  
  item_goto = IupItem("&Goto...\tCtrl+G",NULL);
  IupSetAttribute(item_goto, "IMAGE", "IUP_ArrowRight");
  
  item_cut = IupItem("Cu&t\tCtrl+X",NULL);
  IupSetAttribute(item_cut, "NAME", "ITEM_CUT");
  IupSetAttribute(item_cut, "IMAGE", "IUP_EditCut");
  
  item_copy = IupItem("&Copy\tCtrl+C",NULL);
  IupSetAttribute(item_copy, "NAME", "ITEM_COPY");
  IupSetAttribute(item_copy, "IMAGE", "IUP_EditCopy");
  
  item_paste = IupItem("&Paste\tCtrl+V",NULL);
  IupSetAttribute(item_paste, "NAME", "ITEM_PASTE");
  IupSetAttribute(item_paste, "IMAGE", "IUP_EditPaste");
  
  item_delete = IupItem("&Delete\tDel",NULL);
  IupSetAttribute(item_delete, "NAME", "ITEM_DELETE");
  IupSetAttribute(item_delete, "IMAGE", "IUP_EditErase");
  
  item_select_all = IupItem("Select &All\tCtrl+A",NULL);
  IupSetAttribute(item_select_all, "NAME", "ITEM_SELECT_ALL");
  
  item_help = IupItem("&Help...\tF1", NULL);
  IupSetAttribute(item_help, "IMAGE", "IUP_MessageHelp");
  
  item_codepoint = IupItem("&Unicode Codepoint...",NULL);
  
  item_toolbar = IupItem("&Toolbar", NULL);
  IupSetAttribute(item_toolbar, "VALUE", "ON");
  
  item_statusbar = IupItem("&Statusbar", NULL);
  IupSetAttribute(item_statusbar, "VALUE", "ON");
  
  okbtn = IupButton("&OK", NULL);
  IupSetAttribute(okbtn, "NAME", "OK_BTN");
  IupSetAttribute(okbtn, "ACTIVE", "NO");
  IupSetAttribute(okbtn, "PADDING", "10x2");
  IupSetCallback(okbtn, "ACTION", (Icallback) tedit_okbtn_action_cb);
  IupSetAttribute(okbtn, "IMAGE", "IUP_ActionOK");
  
  cancelbtn = IupButton("&Cancel", NULL);
  IupSetAttribute(cancelbtn, "PADDING", "10x2");
  IupSetCallback(cancelbtn, "ACTION", (Icallback) tedit_cancelbtn_action_cb);
  IupSetAttribute(cancelbtn, "IMAGE", "IUP_ActionCancel");
  
  helpbtn = IupButton("&Help", NULL);
  IupSetAttribute(helpbtn, "PADDING", "10x2");
  IupSetCallback(helpbtn, "ACTION", (Icallback) tedit_item_help_action_cb);
  IupSetAttribute(helpbtn, "IMAGE", "IUP_MessageHelp");
  
  btns_hb = IupHbox(IupFill(), okbtn, cancelbtn, helpbtn, NULL);
  
  btn_find = IupButton(NULL,NULL);
  IupSetAttribute(btn_find, "IMAGE", "IUP_EditFind");
  IupSetAttribute(btn_find, "FLAT", "Yes");
  IupSetAttribute(btn_find, "TIP", "Find (Ctrl+F)");
  IupSetAttribute(btn_find, "CANFOCUS", "No");
  
  btn_goto = IupButton(NULL,NULL);
  IupSetAttribute(btn_goto, "IMAGE", "IUP_ArrowRight");
  IupSetAttribute(btn_goto, "FLAT", "Yes");
  IupSetAttribute(btn_goto, "TIP", "Goto (Ctrl+G)");
  IupSetAttribute(btn_goto, "CANFOCUS", "No");
  
  btn_cut = IupButton(NULL,NULL);
  IupSetAttribute(btn_cut, "IMAGE", "IUP_EditCut");
  IupSetAttribute(btn_cut, "FLAT", "Yes");
  IupSetAttribute(btn_cut, "TIP", "Cut (Ctrl+X)");
  IupSetAttribute(btn_cut, "CANFOCUS", "No");
  
  btn_copy = IupButton(NULL,NULL);
  IupSetAttribute(btn_copy, "IMAGE", "IUP_EditCopy");
  IupSetAttribute(btn_copy, "FLAT", "Yes");
  IupSetAttribute(btn_copy, "TIP", "Copy (Ctrl+C)");
  IupSetAttribute(btn_copy, "CANFOCUS", "No");
  
  btn_paste = IupButton(NULL,NULL);
  IupSetAttribute(btn_paste, "IMAGE", "IUP_EditPaste");
  IupSetAttribute(btn_paste, "FLAT", "Yes");
  IupSetAttribute(btn_paste, "TIP", "Paste (Ctrl+V)");
  IupSetAttribute(btn_paste, "CANFOCUS", "No");
  
  btn_delete = IupButton(NULL,NULL);
  IupSetAttribute(btn_delete, "IMAGE", "IUP_EditErase");
  IupSetAttribute(btn_delete, "FLAT", "Yes");
  IupSetAttribute(btn_delete, "TIP", "Delete (Del)");
  IupSetAttribute(btn_delete, "CANFOCUS", "No");
  
  toolbar_hb = IupHbox(btn_find, btn_goto,
               IupSetAttributes(IupLabel(NULL), "SEPARATOR=VERTICAL"),
               btn_cut, btn_copy, btn_paste, 
               IupSetAttributes(IupLabel(NULL), "SEPARATOR=VERTICAL"), 
               btn_delete, NULL);
  IupSetAttribute(toolbar_hb,"MARGIN","5x5");
  IupSetAttribute(toolbar_hb,"GAP","2");
  
  if (!IupConfigGetVariableIntDef(config, "TextEditor", "Toolbar", 1))
  {
    IupSetAttribute(item_toolbar, "VALUE", "OFF");
    IupSetAttribute(toolbar_hb, "FLOATING", "YES");
    IupSetAttribute(toolbar_hb, "VISIBLE", "NO");
  }
  
  if (!IupConfigGetVariableIntDef(config, "TextEditor", "Statusbar", 1))
  {
    IupSetAttribute(item_statusbar, "VALUE", "OFF");
    IupSetAttribute(lbl_statusbar, "FLOATING", "YES");
    IupSetAttribute(lbl_statusbar, "VISIBLE", "NO");
  }
  
  IupSetCallback(item_cancel, "ACTION", (Icallback) tedit_cancelbtn_action_cb);
  IupSetCallback(item_ok, "ACTION", (Icallback) tedit_okbtn_action_cb);
  IupSetCallback(item_revert, "ACTION", (Icallback) tedit_item_revert_action_cb);
  IupSetCallback(item_find, "ACTION", (Icallback) tedit_item_find_action_cb);
  IupSetCallback(btn_find, "ACTION", (Icallback) tedit_item_find_action_cb);
  IupSetCallback(item_goto, "ACTION", (Icallback) tedit_item_goto_action_cb);
  IupSetCallback(btn_goto, "ACTION", (Icallback) tedit_item_goto_action_cb);
  IupSetCallback(maintext, "CARET_CB", (Icallback) tedit_maintext_caret_cb);
  IupSetCallback(maintext, "VALUECHANGED_CB", 
                 (Icallback) tedit_maintext_valuechanged_cb);
  IupSetCallback(item_cut, "ACTION", (Icallback) tedit_item_cut_action_cb);
  IupSetCallback(btn_cut, "ACTION", (Icallback) tedit_item_cut_action_cb);
  IupSetCallback(item_copy, "ACTION", (Icallback) tedit_item_copy_action_cb);
  IupSetCallback(btn_copy, "ACTION", (Icallback) tedit_item_copy_action_cb);
  IupSetCallback(item_paste, "ACTION", (Icallback) tedit_item_paste_action_cb);
  IupSetCallback(btn_paste, "ACTION", (Icallback) tedit_item_paste_action_cb);
  IupSetCallback(item_delete, "ACTION", (Icallback) tedit_item_delete_action_cb);
  IupSetCallback(btn_delete, "ACTION", (Icallback) tedit_item_delete_action_cb);
  IupSetCallback(item_select_all, "ACTION", (Icallback) tedit_item_select_all_action_cb);
  IupSetCallback(item_codepoint, "ACTION", (Icallback) tedit_insertutf8_action_cb);
  IupSetCallback(item_toolbar, "ACTION", (Icallback) tedit_item_toolbar_action_cb);
  IupSetCallback(item_statusbar, "ACTION", (Icallback) tedit_item_statusbar_action_cb);
  IupSetCallback(item_help, "ACTION", (Icallback) tedit_item_help_action_cb);
  
  contents_menu = IupMenu(
    item_ok,
    IupSeparator(),
    item_cancel,
    IupSeparator(),
    item_revert,
    NULL);
  
  edit_menu = IupMenu(
    item_cut,
    item_copy,
    item_paste,
    IupSeparator(),
    item_delete,
    IupSeparator(),
    item_find,
    item_goto,
    IupSeparator(),
    item_select_all,
    NULL);
  insert_menu = IupMenu(
    item_codepoint,
    NULL);
  view_menu = IupMenu(
    item_toolbar,
    item_statusbar,
    NULL);
  help_menu = IupMenu(
    item_help,
    NULL);
  
  sub_menu_contents = IupSubmenu("&Contents", contents_menu);
  sub_menu_edit = IupSubmenu("&Edit", edit_menu);
  sub_menu_insert = IupSubmenu("&Insert", insert_menu);
  sub_menu_view = IupSubmenu("&View", view_menu);
  sub_menu_help = IupSubmenu("&Help", help_menu);
  
  menu = IupMenu(sub_menu_contents, sub_menu_edit, sub_menu_insert, 
                 sub_menu_view, sub_menu_help, NULL);
  
  vbox = IupVbox(toolbar_hb, maintext, lbl_statusbar, btns_hb, NULL);
  
  dlg = IupDialog(vbox);
  IupSetAttributeHandle(dlg, "MENU", menu);
  
  if (caption) IupSetStrAttribute(dlg, "TITLE", caption);
  else IupSetAttribute(dlg, "TITLE", "Edit Text");
  IupSetAttribute(dlg, "SIZE", "QUARTERxQUARTER");
  IupSetCallback(dlg, "CLOSE_CB", (Icallback) tedit_cancelbtn_action_cb);
  IupSetAttribute(dlg, "CONFIG", (char *) config);
  IupSetAttributeHandle(dlg, "DEFAULTESC", cancelbtn);
  
  IupSetCallback(edit_menu, "OPEN_CB", (Icallback) tedit_edit_menu_open_cb);
  
  IupSetCallback(dlg, "K_cCR", (Icallback) tedit_okbtn_action_cb);
  IupSetCallback(dlg, "K_ESC", (Icallback) tedit_cancelbtn_action_cb);
  IupSetCallback(dlg, "K_F1", (Icallback) tedit_item_help_action_cb);
  IupSetCallback(dlg, "K_cF", (Icallback) tedit_item_find_action_cb);
  IupSetCallback(dlg, "K_cG", (Icallback) tedit_item_goto_action_cb);
  IupSetCallback(dlg, "K_cA", (Icallback) tedit_item_select_all_action_cb);
  IupSetCallback(dlg, "K_DEL", (Icallback) tedit_item_delete_action_cb);
  
  return dlg;
}

int dotedit(Ihandle *config, const char *origintext, char **returntextvar, const char *caption, Ihandle *parentdlg, const char *prevwindowlist)
{
  int ans = 0;
  if (!config || !origintext || !returntextvar) return 0;
  char *windowlist = NULL;
  if (!duplicatewindowlist(&windowlist, prevwindowlist) || 
      addwindowtolist(&windowlist, "tedit"))
  {
    if (windowlist) freewindowlist(&windowlist);
    show_error("Out of Memory!", 1, NULL, parentdlg);
    return 0;
  }
  
  Ihandle *dlg = tedit_makedlg(config, caption);
  if (!dlg)
  {
    freewindowlist(&windowlist);
    show_error("Out of Memory!", 1, NULL, parentdlg);
    return 0;
  }
  IupSetStrAttribute(dlg, "RETURN_TEXT", origintext);
  IupSetAttributeHandle(dlg, "PARENTDIALOG", parentdlg);
  IupSetAttribute(dlg, "WINDOW_LIST", windowlist);
  
  Ihandle *txt = IupGetDialogChild(dlg, "MAIN_TEXT");
  IupSetStrAttribute(txt, "VALUE", origintext);
  
  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
  
  if (IupGetInt(dlg, "STATUS") == 1)
  {
    char *returntext = IupGetAttribute(dlg, "RETURN_TEXT");
    if (returntext)
    {
      *returntextvar = (char *) malloc(sizeof(char) * (1+strlen(returntext)));
      if (*returntextvar)
      {
        strcpy(*returntextvar, returntext);
        ans = 1; // ans is only 1 if *returntextvar is set!
      }
      else
        show_error("Out of Memory!", 1, NULL, parentdlg);
    }
    else
    {
      show_error("Error returning text!", 1, NULL, parentdlg);
    }
  }
  IupSetAttribute(dlg, "WINDOW_LIST", NULL);
  freewindowlist(&windowlist);
  IupDestroy(dlg);
    
  return ans;
  
}
