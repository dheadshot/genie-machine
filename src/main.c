#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iup/iup.h>
#include <iup/iup_config.h>
#include <errno.h>
#include <sqlite3.h>
#include "safeiupconf.h"
#include "utf8.h"
#include "proginfo.h"
#include "dbaccess.h"
#include "types.h"

#include "main.h"


/*----------------- Helper Functions ---------------- */

const char *str_fileext(const char *filename)
{
  /* Start at last char and work backwards */
      	fprintf(stderr, "--FN = %s--\n", filename);
  long len = (long) strlen(filename);
      	fprintf(stderr, "--LEN(FN) = %ld--\n", len);
  long offset = len - 1;
      	fprintf(stderr, "--OFFSET = %ld--\n", offset);
  while (offset > 0)
  {
    if (filename[offset] == '\\' || filename[offset] == '/') break;
    
    if (filename[offset] == '.')
    {
      offset++;
      	fprintf(stderr, "--EXT = '%s'--\n",filename + (sizeof(char)*offset));
      return filename + (sizeof(char)*offset);
    }
    offset--;
  }
      	fprintf(stderr, "--EXT = NULL--\n");
  return NULL;
}

int selectfile(Ihandle *parentdlg, int isopen)
{
  Ihandle *config = (Ihandle *) IupGetAttribute(parentdlg, "CONFIG");
  const char *dir = IupConfigGetVariableStr(config, "MainWindow", "LastDirectory");
  int ans = 1;
  
  Ihandle *filedlg = IupFileDlg();
  
  if (isopen) IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
  else
  {
    IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");
    IupSetStrAttribute(filedlg, "FILE", NULL);//IupGetAttribute(parentdlg, "FILENAME"));
  }
  IupSetAttribute(filedlg, "EXTFILTER", "Genie-Machine Databases|*.gendb|Databases|*.db|All Files|*.*|");
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", parentdlg);
  
  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char *filename = IupGetAttribute(filedlg, "VALUE");
      	fprintf(stderr, "--FN = %s--\n",filename);
    char *errmsg = NULL;
    int extnum = IupGetInt(filedlg, "FILTERUSED");
      	fprintf(stderr, "--EXTNUM = %d--\n",extnum);
    int rc = 0;
    int fileexists = 0;
    
    if (!isopen && !str_fileext(filename) && extnum < 3)
    {
      char *tmpfn = (char *) malloc(sizeof(char)*(10+strlen(filename)));
      if (!tmpfn)
      {
        IupMessageError(parentdlg,"Error opening file: Out of Memory!");
        rc = 1;
      }
      else if (extnum == 1)
      {
        /* .gendb */
        strcpy(tmpfn,filename);
        strcat(tmpfn,".gendb");
      }
      else
      {
        /* .db */
        strcpy(tmpfn,filename);
        strcat(tmpfn,".db");
      }
      if (!rc)
      {
        IupSetStrAttribute(parentdlg, "AMENDED_FILEDLG_VALUE", tmpfn);
        filename = IupGetAttribute(parentdlg, "AMENDED_FILEDLG_VALUE");
        free(tmpfn);
      }
    }
    
    if (!rc) fileexists = afnexists(filename);
    
    if (rc || (isopen && !fileexists))
    {
      if (!rc)
      {
        	fprintf(stderr, "--Error: file '%s' doesn't exist!--\n");
        errmsg = (char *) malloc(sizeof(char)*(101+strlen(filename)));
        if (errmsg)
        {
          sprintf(errmsg, "Error opening file: the file \"%s\" does not exist!", filename);
          IupMessageError(parentdlg,errmsg);
          free(errmsg);
        }
        else
        {
          IupMessageError(parentdlg,"Error opening file: the file does not exist!\nAdditionally, an \"Out of Memory\" error was encountered processing the above error!");
        }
      }
    }
    else
    {
      if (!isopen && fileexists)
      {
        /* Wipe File */
        FILE *af;
        af = fopen(filename, "w");
        if (af) fclose(af);
        	fprintf(stderr, "--Wiped FN = %s--\n",filename);
      }
      /* Open/Create File */
      if (isopen) rc = opendb(filename, 0); else rc = opendb(filename, 1);
        	fprintf(stderr,"--Opened file--\n");
      if (rc != 1)
      {
        	fprintf(stderr,"--Error: %lu with file %s--\n",getlastdberr(),filename);
        errmsg = (char *) malloc(sizeof(char)*(101+strlen(filename)));
        if (errmsg)
        {
          sprintf(errmsg,"There was an Error %lu opening the database \"%s\"!",getlastdberr(), filename);
          IupMessageError(parentdlg,errmsg);
          free(errmsg);
        }
        else
        {
          IupMessageError(parentdlg,"There was an error opening the database.  Additionally, there was an error responding to this error!");
        }
        ans = 0;
      }
      else
      {
        dir = IupGetAttribute(filedlg, "DIRECTORY");
        IupConfigSetVariableStr(config, "MainWindow", "LastDirectory", dir);
        IupSetStrAttribute(parentdlg, "FILENAME", filename);
        IupConfigRecentUpdate(config, filename);
        ans = 2;
      }
    }
  }
  
  IupDestroy(filedlg);
  IupSetAttribute(parentdlg, "AMENDED_FILEDLG_VALUE", NULL);
  	fprintf(stderr, "--Finished with Select File--\n");
  return ans;/*IUP_DEFAULT;*/
}

int populatemainlists(Ihandle *ih)
{
  Ihandle *sourcelist, *relationshiplist, *personlist;
  ntsa plist;
  char *errmsg;
  personlist = IupGetDialogChild(ih, "PERSONLIST");
  
  IupSetAttribute(personlist, "1", NULL);
  plist = getdb_mainpersonlist();
  if (plist == NULL)
  {
    errmsg = (char *) malloc(sizeof(char) + (101+sstrlen(lastdberrtext)));
    if (errmsg == NULL)
    {
      /* Show a default error message */
      IupMessageError(IupGetDialog(ih), "There was an error accessing the database and an additional error generating the error message!");
    }
    else
    {
      if (lastdberrtext)
        sprintf(errmsg, "There was an error accessing the database:\n %lu:%lu %s", getlastdberr(), lastdberl, lastdberrtext);
      else
        sprintf(errmsg, "There was an error '%lu:%lu' accessing the database!", getlastdberr(), lastdberl);
      /* Show the error message */
      IupMessageError(IupGetDialog(ih), errmsg);
      free(errmsg);
    }
    return 0;
  }
  
  sqlite3_int64 i;
  
  for (i=0; plist[i] != NULL; i++)
  {
    IupSetStrAttribute(personlist, "APPENDITEM", plist[i]);
    free(plist[i]);
  }
  
  free(plist);
  plist = NULL;
}

void donewfile(Ihandle *ih)
{
  if (selectfile(IupGetDialog(ih), 0) == 2)
  {
  	fprintf(stderr, "--Initialising File--\n");
    if (initnewdb() != 1)
    {
      /* Compile and show Error Message */
  	fprintf(stderr, "--Error Initialising DB!--\n");
  	fprintf(stderr, "--Error '%lu:%lu': %s--\n",getlastdberr(),lastdberl,lastdberrtext);
      char *errmsg = (char *) malloc(sizeof(char)*(101+sstrlen(lastdberrtext)));
      if (errmsg)
      {
        sprintf(errmsg, "Error '%lu:%lu' Initialising Database:\n%s", getlastdberr(), lastdberl, lastdberrtext);
        IupMessageError(IupGetDialog(ih), errmsg);
        free(errmsg);
        
      }
      else
      {
        IupMessageError(IupGetDialog(ih), "Error Initialising Database!  Additionally, error processing this error!");
      }
    }
  	fprintf(stderr, "--Finished Initialising File--\n");
    
  }
}

/* ---------------- Callback Functions ---------------- */

int item_exit_action_cb(Ihandle *item_exit)
{
  Ihandle *dlg = IupGetDialog(item_exit);
  Ihandle *config = (Ihandle *) IupGetAttribute(dlg, "CONFIG");
  
  /* "Are you sure you wish to exit?" */
  char *filename = IupGetAttribute(dlg, "FILENAME");
  if (filename && filename[0] && IupAlarm("Warning", 
      "Are you sure you want to exit Genie Machine?", "Yes", "No", NULL) != 1)
  {
    return IUP_DEFAULT;
  }
  
  closedb();
  IupConfigDialogClosed(config, dlg, "MainWindow");
  IupConfigSave(config);
  IupDestroy(config);
  return IUP_CLOSE;
}

int item_new_action_cb(Ihandle *item_new)
{
//  if (IupGetAttribute(IupGetDialog(item_new),"FILENAME")) /* Close file */
  donewfile(item_new);
  if (IupGetAttribute(IupGetDialog(item_new),"FILENAME"))
  {
    Ihandle *close_btn = IupGetDialogChild(item_new,"CLOSE_BTN");
    IupSetAttribute(close_btn, "ACTIVE", "YES");
  }
  return IUP_DEFAULT;
}

int config_recent_cb(Ihandle *ih)
{
  Ihandle *dlg = IupGetDialog(ih);
  Ihandle *config = (Ihandle *) IupGetAttribute(dlg, "CONFIG");
  
  char *filename = IupGetAttribute(dlg, "FILENAME");
  if (filename && filename[0] && IupAlarm("Warning", 
      "Are you sure you want to close the current file and open another?", "Yes", "No", NULL) != 1)
  {
    return IUP_DEFAULT;
  }
  
  filename = IupGetAttribute(ih, "TITLE");
  /* TODO: Write routine to open the file in filename! */
  int fileexists = afnexists(filename);
  char *errmsg;
  
  if (!fileexists)
  {
    	fprintf(stderr, "--Error: file '%s' doesn't exist!--\n");
    errmsg = (char *) malloc(sizeof(char)*(101+strlen(filename)));
    if (errmsg)
    {
      sprintf(errmsg, "Error opening file: the file \"%s\" does not exist!", filename);
      IupMessageError(dlg,errmsg);
      free(errmsg);
    }
    else
    {
      IupMessageError(dlg,"Error opening file: the file does not exist!\nAdditionally, an \"Out of Memory\" error was encountered processing the above error!");
    }
  }
  else
  {
    int rc = opendb(filename, 0);
        	fprintf(stderr,"--Opened file--\n");
    if (rc != 1)
    {
      	fprintf(stderr,"--Error: %lu with file %s--\n",getlastdberr(),filename);
      errmsg = (char *) malloc(sizeof(char)*(101+strlen(filename)));
      if (errmsg)
      {
        sprintf(errmsg,"There was an Error %lu opening the database \"%s\"!",getlastdberr(), filename);
        IupMessageError(dlg,errmsg);
        free(errmsg);
      }
      else
      {
        IupMessageError(dlg,"There was an error opening the database.  Additionally, there was an error responding to this error!");
      }
//      ans = 0;
    }
    else
    {
//      char *dir = IupGetAttribute(filedlg, "DIRECTORY");
//      IupConfigSetVariableStr(config, "MainWindow", "LastDirectory", dir);
      IupSetStrAttribute(dlg, "FILENAME", filename);
      IupConfigRecentUpdate(config, filename);
//      ans = 2;
      Ihandle *close_btn = IupGetDialogChild(ih, "CLOSE_BTN");
      IupSetAttribute(close_btn,"ACTIVE", "YES");
      populatemainlists(ih);
    }
  }
  
  return IUP_DEFAULT;
}

int file_menu_open_cb(Ihandle *ih)
{
  Ihandle *item_close = IupGetDialogChild(ih, "ITEM_CLOSE");
  Ihandle *item_merge = IupGetDialogChild(ih, "ITEM_MERGE");
  Ihandle *item_export_gedcom55 = IupGetDialogChild(ih, "ITEM_EXPORT_GEDCOM55");
  Ihandle *item_export_gedcom551 = IupGetDialogChild(ih, "ITEM_EXPORT_GEDCOM551");
  char *filename = IupGetAttribute(IupGetDialog(ih), "FILENAME");
  
  if (filename && filename[0])
  {
    IupSetAttribute(item_close, "ACTIVE", "YES");
    IupSetAttribute(item_merge, "ACTIVE", "YES");
    IupSetAttribute(item_export_gedcom55, "ACTIVE", "YES");
    IupSetAttribute(item_export_gedcom551, "ACTIVE", "YES");
  }
  else
  {
    IupSetAttribute(item_close, "ACTIVE", "NO");
    IupSetAttribute(item_merge, "ACTIVE", "NO");
    IupSetAttribute(item_export_gedcom55, "ACTIVE", "NO");
    IupSetAttribute(item_export_gedcom551, "ACTIVE", "NO");
  }
  
  return IUP_DEFAULT;
}

/* ---------------- Main Functions ----------------- */

Ihandle *create_mainwindow_menu(Ihandle *config)
{
  Ihandle *menu, *recent_menu;
  Ihandle *file_menu, *file_submenu, *item_new, *item_open, *item_close, 
          *item_merge, *import_menu, *import_submenu, *item_import_gedcom, 
          *export_menu, *export_submenu, *item_export_gedcom55, 
          *item_export_gedcom551, *item_exit;
  Ihandle *person_menu, *person_submenu, *item_newperson, *item_editperson, 
          *item_viewperson, *item_deleteperson, *item_ancestraltree, 
          *item_descendenttreefromperson;
  Ihandle *relationship_menu, *relationship_submenu, *item_newrelationship, 
          *item_editrelationship, *item_viewrelationship, 
          *item_deleterelationship, *item_descendenttreefromrelationship;
  Ihandle *source_menu, *source_submenu, *item_newsource, *item_editsource, 
          *item_viewsource, *item_deletesource;
  Ihandle *view_menu, *view_submenu, *item_showtoolbar, *item_showstatusbar;
  Ihandle *help_menu, *help_submenu, *item_help, *item_setbrowser, *item_about;
  
  item_new = IupItem("&New...\tCtrl+N", NULL); /* Dunno if it's a good idea to have a shortcut for New */
  IupSetAttribute(item_new, "IMAGE", "IUP_FileNew");
  IupSetCallback(item_new, "ACTION", (Icallback) item_new_action_cb);
  
  item_open = IupItem("&Open...\tCtrl+O", NULL);
  IupSetAttribute(item_open, "IMAGE", "IUP_FileOpen");
  
  item_close = IupItem("&Close", NULL);
  IupSetAttribute(item_close, "NAME", "ITEM_CLOSE");
  IupSetAttribute(item_close, "IMAGE", "IUP_FileClose");
  IupSetAttribute(item_close, "ACTIVE", "NO");
  
  item_merge = IupItem("&Merge...", NULL);
  IupSetAttribute(item_merge, "NAME", "ITEM_MERGE");
  IupSetAttribute(item_merge, "ACTIVE", "NO");
  
  item_import_gedcom = IupItem("Import &GEDCOM File...", NULL);
  
  item_export_gedcom55 = IupItem("Export GEDCOM &5.5 File...", NULL);
  IupSetAttribute(item_export_gedcom55, "NAME", "ITEM_EXPORT_GEDCOM55");
  IupSetAttribute(item_export_gedcom55, "ACTIVE", "NO");
  
  item_export_gedcom551 = IupItem("Export GEDCOM 5.5.&1 File...", NULL);
  IupSetAttribute(item_export_gedcom551, "NAME", "ITEM_EXPORT_GEDCOM551");
  IupSetAttribute(item_export_gedcom55, "ACTIVE", "NO");
  
  item_exit = IupItem("E&xit", NULL);
  IupSetAttribute(item_exit, "NAME", "ITEM_EXIT");
  IupSetCallback(item_exit, "ACTION", (Icallback) item_exit_action_cb);
  
  item_showtoolbar = IupItem("Show &Toolbar", NULL);
  IupSetAttribute(item_showtoolbar, "NAME", "ITEM_SHOWTOOLBAR");
  IupSetAttribute(item_showtoolbar, "VALUE", "ON");
  
  item_showstatusbar = IupItem("Show &Statusbar", NULL);
  IupSetAttribute(item_showstatusbar, "NAME", "ITEM_SHOWSTATUSBAR");
  IupSetAttribute(item_showstatusbar, "VALUE", "ON");
  
  item_help = IupItem("Help...\tF1", NULL);
  
  item_setbrowser = IupItem("Set &Web-Browser...", NULL);
  
  item_about = IupItem("About...", NULL);
  
  recent_menu = IupMenu(NULL);
  
  import_menu = IupMenu(
                        item_import_gedcom,
                        NULL);
  
  export_menu = IupMenu(
                        item_export_gedcom55,
                        item_export_gedcom551,
                        NULL);
  
  import_submenu = IupSubmenu("&Import", import_menu);
  export_submenu = IupSubmenu("&Export", export_menu);
  
  file_menu = IupMenu(
                      item_new,
                      item_open,
                      item_close,
                      IupSeparator(),
                      IupSubmenu("Recent &Files", recent_menu),
                      IupSeparator(),
                      item_merge,
                      IupSeparator(),
                      import_submenu,
                      export_submenu,
                      IupSeparator(),
                      item_exit,
                      NULL);
  
  view_menu = IupMenu(
                      item_showtoolbar,
                      item_showstatusbar,
                      NULL);
  
  help_menu = IupMenu(
                      item_help,
                      item_setbrowser,
                      item_about,
                      NULL);
  
  file_submenu = IupSubmenu("&File", file_menu);
  view_submenu = IupSubmenu("&View", view_menu);
  help_submenu = IupSubmenu("&Help", help_menu);
  
  menu = IupMenu(
                 file_submenu,
                 view_submenu,
                 help_submenu,
                 NULL);
  
  IupSetCallback(file_menu, "OPEN_CB", (Icallback) file_menu_open_cb);
  
  /* Initialise variables from the Config file */
  IupConfigRecentInit(config, recent_menu, config_recent_cb, 10);
  
  if (!IupConfigGetVariableIntDef(config, "MainWindow", "Toolbar", 1))
  {
    IupSetAttribute(item_showtoolbar, "VALUE", "OFF");
  }
  
  if (!IupConfigGetVariableIntDef(config, "MainWindow", "Statusbar", 1))
  {
    IupSetAttribute(item_showstatusbar, "VALUE", "OFF");
  }
  
  return menu;
}

Ihandle *create_toolbar(Ihandle *config)
{
  Ihandle *toolbar;
  Ihandle *new_btn, *open_btn, *close_btn;
  
  new_btn = IupButton(NULL, NULL);
  IupSetAttribute(new_btn, "IMAGE", "IUP_FileNew");
  IupSetAttribute(new_btn, "FLAT", "YES");
  IupSetAttribute(new_btn, "TIP", "Create a New File... (Ctrl+N)");
  IupSetAttribute(new_btn, "CANFOCUS", "NO");
  IupSetCallback(new_btn, "ACTION", (Icallback) item_new_action_cb);
  
  open_btn = IupButton(NULL, NULL);
  IupSetAttribute(open_btn, "IMAGE", "IUP_FileOpen");
  IupSetAttribute(open_btn, "FLAT", "YES");
  IupSetAttribute(open_btn, "TIP", "Open Existing File... (Ctrl+O)");
  IupSetAttribute(open_btn, "CANFOCUS", "NO");
  
  close_btn = IupButton(NULL, NULL);
  IupSetAttribute(close_btn, "IMAGE", "IUP_FileClose");
  IupSetAttribute(close_btn, "FLAT", "YES");
  IupSetAttribute(close_btn, "TIP", "Close Current File");
  IupSetAttribute(close_btn, "CANFOCUS", "NO");
  IupSetAttribute(close_btn, "NAME", "CLOSE_BTN");
  IupSetAttribute(close_btn, "ACTIVE", "NO");
  
  toolbar = IupHbox(
                    new_btn,
                    open_btn,
                    close_btn,
                    IupSetAttributes(IupLabel(NULL), "SEPARATOR=VERTICAL"),
                    NULL);
  IupSetAttribute(toolbar, "MARGIN", "5x5");
  IupSetAttribute(toolbar, "GAP", "2");
  IupSetAttribute(toolbar, "NAME", "TOOLBAR");
  
  /* Initialise values from config file */
  if (!IupConfigGetVariableIntDef(config, "MainWindow", "Toolbar", 1))
  {
    IupSetAttribute(toolbar, "FLOATING", "YES");
    IupSetAttribute(toolbar, "VISIBLE", "NO");
  }
  
  return toolbar;
}

Ihandle *create_statusbar(Ihandle *config)
{
  Ihandle *statusbar;
  
  statusbar = IupHbox(
    IupSetAttributes(IupLabel("-"),
      "EXPAND=HORIZONTAL, NAME=CURRENTDB_LABEL, PADDING=10x5"),
    IupSetAttributes(IupLabel(NULL), "SEPARATOR=VERTICAL"),
    IupSetAttributes(IupLabel(" "),
      "EXPAND=HORIZONTAL, PADDING=10x5, NAME=DETAILS_LABEL, ELLIPSIS=YES"),
    NULL);
  IupSetAttribute(statusbar, "NAME", "STATUSBAR");
  
  /* Initialise variables from config file */
  if (!IupConfigGetVariableIntDef(config, "MainWindow", "Statusbar", 1))
  {
    IupSetAttribute(statusbar, "FLOATING", "YES");
    IupSetAttribute(statusbar, "VISIBLE", "NO");
  }
  
  return statusbar;
}

Ihandle *create_mainwindow(Ihandle *config)
{
  Ihandle *dlg, *vbox, *hbox, *menu, *statusbar, *toolbar;
  Ihandle *person_list, *relationship_list, *source_list;
  Ihandle *personlist_label, *relationshiplist_label, *sourcelist_label;
  
  personlist_label = IupLabel("People:");
  relationshiplist_label = IupLabel("Relationships:");
  sourcelist_label = IupLabel("Sources:");
  
  person_list = IupList(NULL);
  IupSetAttribute(person_list, "NAME", "PERSONLIST");
  IupSetAttribute(person_list, "1", NULL);
  IupSetAttribute(person_list, "DROPDOWN", "NO");
  IupSetAttribute(person_list, "SORT", "YES");
  IupSetAttribute(person_list, "ACTIVE", "NO");
  IupSetAttribute(person_list, "EXPAND", "YES");
  
  relationship_list = IupList(NULL);
  IupSetAttribute(relationship_list, "NAME", "RELATIONSHIPLIST");
  IupSetAttribute(relationship_list, "1", NULL);
  IupSetAttribute(relationship_list, "DROPDOWN", "NO");
  IupSetAttribute(relationship_list, "SORT", "NO");
  IupSetAttribute(relationship_list, "ACTIVE", "NO");
  IupSetAttribute(relationship_list, "EXPAND", "YES");
  
  source_list = IupList(NULL);
  IupSetAttribute(source_list, "NAME", "SOURCELIST");
  IupSetAttribute(source_list, "1", NULL);
  IupSetAttribute(source_list, "DROPDOWN", "NO");
  IupSetAttribute(source_list, "SORT", "NO");
  IupSetAttribute(source_list, "ACTIVE", "NO");
  IupSetAttribute(source_list, "EXPAND", "YES");
  
  hbox = IupHbox(
                 IupVbox(
                         personlist_label,
                         person_list,
                         NULL),
                 IupVbox(
                         relationshiplist_label,
                         relationship_list,
                         NULL),
                 IupVbox(
                         sourcelist_label,
                         source_list,
                         NULL),
                 NULL);
  IupSetAttribute(hbox, "GAP","5");
  IupSetAttribute(hbox, "EXPAND","YES");
  
  toolbar = create_toolbar(config);
  statusbar = create_statusbar(config);
  
  vbox = IupVbox(
                 toolbar,
                 hbox,
                 statusbar,
                 NULL);
  
  /* Create menu */
  menu = create_mainwindow_menu(config);
  
  /* Make Dlg */
  dlg = IupDialog(vbox);
  IupSetAttributeHandle(dlg, "MENU", menu);
  IupSetAttribute(dlg, "SIZE", "HALFxHALF");
  IupSetAttribute(dlg, "TITLE", "The Genie Machine");
  IupSetCallback(dlg, "CLOSE_CB", (Icallback) item_exit_action_cb);
  
  /* Keyboard Shortcuts! */
  
  /* Parent for predefined dialogs in closed functions */
  IupSetAttributeHandle(NULL, "PARENTDIALOG", dlg);
  
  /* Set Help Program */
  const char *helpapp;
  
  helpapp = IupConfigGetVariableStr(config, "MainWindow", "HelpPrg");
  if (helpapp)
  {
    IupSetStrAttribute(NULL, "HELPAPP", helpapp);
  }
  
  /* Store Config */
  IupSetAttribute(dlg, "CONFIG", (char *) config);
  
  return dlg;
}

int main(int argc, char *argv[])
{
  Ihandle *config, *dlg;
  
  IupOpen(&argc, &argv);
  IupImageLibOpen();
  
  config = IupConfig();
  IupSetAttribute(config, "APP_NAME", PROG_NAME);
  IupSetAttribute(config, "APP_CONFIG", "NO");
  SafeIupConfigLoad(config);
  
  dlg = create_mainwindow(config);
  
  IupConfigDialogShow(config, dlg, "MainWindow");
  
  IupMainLoop();
  
  IupClose();
  return EXIT_SUCCESS;
}
