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


int tfb_find_next_action_cb(Ihandle *bt_next)
{
  Ihandle *dlg = IupGetDialog(bt_next);
  Ihandle *mtext = (Ihandle *) IupGetAttribute(dlg, "MAIN_TEXT_HANDLE");
  char *str = IupGetAttribute(mtext, "VALUE");
  int find_posc = IupGetInt(mtext, "FIND_POSCHAR");
  int find_posb = IupGetInt(mtext, "FIND_POSBYTE");
  Ihandle *txt = IupGetDialogChild(bt_next, "FIND_TEXT");
  char *str_to_find = IupGetAttribute(txt, "VALUE");
  Ihandle *find_case = IupGetDialogChild(bt_next, "FIND_CASE");
  int casesensitive = IupGetInt(find_case, "VALUE");
  
  long *pos = str_find(str+find_posb, str_to_find, casesensitive);
  if (pos == NULL)
  {
    	fprintf(stderr, "Out Of Memory!");
    show_error("Out of Memory!", 1, NULL, dlg);
    return IUP_DEFAULT;
  }
  if (pos[0]>=0 && pos[1]>=0)
  {
    pos[0] += (long) find_posb;
    pos[1] += (long) find_posc;
  }
  else if (find_posc > 0)
  {
    free(pos);
    pos = str_find(str, str_to_find, casesensitive); /* Try again from the start */
    if (pos == NULL)
    {
      	fprintf(stderr, "Out Of Memory!");
      show_error("Out of Memory!", 1, NULL, dlg);
      return IUP_DEFAULT;
    }
  }
  
  if (pos[0] >= 0 && pos[1] >= 0)
  {
    //strcharcount
    int lin, col;
    long end_posc = pos[1] + strcharcount(str_to_find);
    long end_posb = pos[0] + ((long) strlen(str_to_find));
    
    IupSetInt(mtext, "FIND_POSBYTE", ((int) end_posb));
    IupSetInt(mtext, "FIND_POSCHAR", ((int) end_posc));
    
    IupSetFocus(mtext);
    IupSetfAttribute(mtext, "SELECTIONPOS", "%d:%d", ((int) pos[1]), ((int) end_posc));
    
    IupTextConvertPosToLinCol(mtext, ((int) pos[1]), &lin, &col);
    IupTextConvertLinColToPos(mtext, lin, 0, ((int *) &(pos[1])));  /* Just scroll lines */
    IupSetInt(mtext, "SCROLLTOPOS", ((int) pos[1]));
  }
  else
  {
    /* IupMessage("Warning", "Text not found."); */
    show_error("Text not found.", 0, "Warning", dlg);
  }
  free(pos);
  return IUP_DEFAULT;
}

int tfb_find_close_action_cb(Ihandle *bt_close)
{
  IupHide(IupGetDialog(bt_close));
  return IUP_DEFAULT;
}

Ihandle *tfb_create(Ihandle *config, Ihandle *maintext, Ihandle *parentdlg)
{
  Ihandle *dlg, *box, *bt_next, *bt_close, *txt, *find_case;
  
  txt = IupText(NULL);
  IupSetAttribute(txt, "NAME", "FIND_TEXT");
  IupSetAttribute(txt, "VISIBLECOLUMNS", "20");
  
  find_case = IupToggle("Case &Sensitive", NULL);
  IupSetAttribute(find_case, "NAME", "FIND_CASE");
  
  bt_next = IupButton("Find &Next", NULL);
  IupSetAttribute(bt_next, "PADDING", "10x2");
  IupSetCallback(bt_next, "ACTION", (Icallback)tfb_find_next_action_cb);
  
  bt_close = IupButton("&Close", NULL);
  IupSetAttribute(bt_close, "PADDING", "10x2");
  IupSetCallback(bt_close, "ACTION", (Icallback)tfb_find_close_action_cb);
  
  box = IupVbox(
    IupLabel("Find What:"),
    txt,
    find_case,
    IupSetAttributes(IupHbox(
      IupFill(),
      bt_next,
      bt_close,
      NULL),"NORMALIZESIZE=HORIZONTAL"),
    NULL);
  IupSetAttribute(box, "MARGIN", "10x10");
  IupSetAttribute(box, "GAP", "5");
  
  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "Find");
  IupSetAttribute(dlg, "DIALOGFRAME", "YES");
  IupSetAttribute(dlg, "MAIN_TEXT_HANDLE", (char *) maintext);
  IupSetAttribute(dlg, "CONFIG", (char *) config);
  IupSetAttributeHandle(dlg, "DEFAULTENTER", bt_next);
  IupSetAttributeHandle(dlg, "DEFAULTESC", bt_close);
  IupSetAttributeHandle(dlg, "PARENTDIALOG", parentdlg);
  
  return dlg;
}

void tfb_create_or_show(Ihandle *parentdlg, Ihandle *maintext, Ihandle *config, char *prevwindowlist)
{
  Ihandle *dlg = (Ihandle *) IupGetAttribute(parentdlg, "FIND_DIALOG");
  if (!dlg)
  {
    dlg = tfb_create(config, maintext, parentdlg);
    IupSetStrAttribute(dlg, "PREVIOUS_WINDOW_LIST", prevwindowlist);
    
    /* Save dialogue to reuse it */
    IupSetAttribute(parentdlg, "FIND_DIALOG", (char *) dlg);
  }
  
  /* Current position is CENTER_PARENT at first, then previous value */
  IupShowXY(dlg, IUP_CURRENT, IUP_CURRENT);
}

void tfb_free(Ihandle *dlg)
{
  IupSetAttribute(dlg, "CONFIG", NULL);
  IupSetAttribute(dlg, "MAIN_TEXT_HANDLE", NULL);
  IupDestroy(dlg);
}
