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

int error_filedoesnotexist(const char *filename, Ihandle *parentdlg) /* Parentdlg can be NULL */
{
  char *errmsg = NULL;
  
  	fprintf(stderr, "--Error: file '%s' doesn't exist!--\n");
  errmsg = (char *) malloc(sizeof(char)*(61+strlen(filename)));
  if (errmsg)
  {
    sprintf(errmsg, "Error opening file: the file \"%s\" does not exist!", filename);
    show_error(errmsg, 1, "File Access Error", parentdlg);
    free(errmsg);
    return 1;
  }
  else
  {
    show_error("Error opening file: the file does not exist!\nAdditionally, an \"Out of Memory\" error was encountered processing the above error!", 1, NULL, parentdlg);
    return 0;
  }
}

int error_openingdb(const char *filename, const unsigned long errnum, Ihandle *parentdlg) /* Parentdlg can be NULL */
{
  char *errmsg = NULL;
  
  	fprintf(stderr,"--Error: %lu with file %s--\n",errnum,filename);
  errmsg = (char *) malloc(sizeof(char)*(101+strlen(filename)));
  if (errmsg)
  {
    sprintf(errmsg,"There was an Error %lu opening the database \"%s\"!",errnum, filename);
    show_error(errmsg, 1, "Database Opening Error", parentdlg);
    free(errmsg);
    return 1;
  }
  else
  {
    show_error("There was an error opening the database.  Additionally, there was an error responding to this error!", 1, NULL, parentdlg);
    return 0;
  }
}

int error_accessingdb(const unsigned long errnum, const unsigned long errline, const char *errtext, Ihandle *parentdlg) /* Parentdlg can be NULL */
{
  char *errmsg = NULL;
  
  errmsg = (char *) malloc(sizeof(char) + (101+sstrlen(errtext)));
  if (errmsg == NULL)
  {
    /* Show a default error message */
    show_error("There was an error accessing the database and an additional error generating the error message!", 1, NULL, parentdlg);
    return 0;
  }
  else
  {
    if (errtext)
      sprintf(errmsg, "There was an error accessing the database:\n %lu:%lu %s", errnum, errline, errtext);
    else
      sprintf(errmsg, "There was an error '%lu:%lu' accessing the database!", errnum, errline);
    /* Show the error message */
    show_error(errmsg, 1, "Database Access Error", parentdlg);
    free(errmsg);
  }
  return 1;
}

int error_initdb(const unsigned long errnum, const unsigned long errline, const char *errtext, Ihandle *parentdlg) /* Parentdlg can be NULL */
{
  char *errmsg = NULL;
  
  /* Compile and show Error Message */
  	fprintf(stderr, "--Error Initialising DB!--\n");
  	fprintf(stderr, "--Error '%lu:%lu': %s--\n",errnum,errline,errtext);
  errmsg = (char *) malloc(sizeof(char)*(101+sstrlen(errtext)));
  if (errmsg)
  {
    sprintf(errmsg, "Error '%lu:%lu' Initialising Database:\n%s", errnum, errline, errtext);
    show_error(errmsg, 1, "Database Initialisation Error", parentdlg);
    free(errmsg);
    return 1;
  }
  else
  {
    show_error("Error Initialising Database!  Additionally, error processing this error!", 1, NULL, parentdlg);
    return 0;
  }
}

int error_closingdb(const unsigned long errnum, Ihandle *parentdlg) /* Parentdlg can be NULL */
{
  char *errmsg = (char *) malloc(sizeof(char)*101);
  if (errmsg)
  {
    sprintf(errmsg,"Error %d Encountered Closing Database!",errnum);
    show_error(errmsg, 1, "Database Closing Error", parentdlg);
    free(errmsg);
    return 1;
  }
  else
  {
    show_error("Error Encountered Closing Database!\nAdditionally, another error was encountered processing this error.", 1, NULL, parentdlg);
    return 0;
  }
}
