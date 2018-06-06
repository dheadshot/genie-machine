#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iup/iup.h>
#include <iup/iup_config.h>
#include <errno.h>

#include "utf8.h"

#include "doerror.h"

void show_error(const char *message, int is_error, const char *title, Ihandle *parentdlg) /* Title and parentdlg can be NULL */
{
  Ihandle *dlg = IupMessageDlg();
  char *dlgtitle = NULL;
  
  if (title && title[0])
  {
    dlgtitle = malloc(sizeof(char)*(10+strlen(title)));
    if (dlgtitle)
    {
      if (is_error) strcpy(dlgtitle, "Error: ");
      else strcpy(dlgtitle, "Warning: ");
      strcat(dlgtitle, title);
    }
  }
  
  if (parentdlg) IupSetAttributeHandle(dlg, "PARENTDIALOG", parentdlg); //Might fail here?
  else IupSetStrAttribute(dlg, "PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  if (is_error)
  {
    IupSetAttribute(dlg, "DIALOGTYPE", "ERROR");
    if (dlgtitle) IupSetStrAttribute(dlg, "TITLE", dlgtitle);
    else IupSetStrAttribute(dlg, "TITLE", "Error");
  }
  else
  {
    IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
    if (dlgtitle) IupSetStrAttribute(dlg, "TITLE", dlgtitle);
    else IupSetStrAttribute(dlg, "TITLE", "Warning");
  }
  IupSetAttribute(dlg, "BUTTONS", "OK");
  IupSetStrAttribute(dlg, "VALUE", message);
  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
  IupDestroy(dlg);
  if (dlgtitle) free(dlgtitle);
}