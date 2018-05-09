#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iup/iup.h>
#include <iup/iup_config.h>
#include <errno.h>
#include "safeiupconf.h"
#include "utf8.h"
#include "proginfo.h"
#include "dbaccess.h"

#include "main.h"


/* ---------------- Callback Functions ---------------- */

int item_exit_action_cb(Ihandle *item_exit)
{
  Ihandle *dlg = IupGetDialog(item_exit);
  Ihandle *config = (Ihandle *) IupGetAttribute(dlg, "CONFIG");
  
  /* TODO: "Are you sure you wish to exit?" */
  
  closedb();
  IupConfigDialogClosed(config, dlg, "MainWindow");
  IupConfigSave(config);
  IupDestroy(config);
  return IUP_CLOSE;
}

/* ---------------- Main Functions ----------------- */

Ihandle *create_mainwindow_menu(Ihandle *config)
{
  Ihandle *menu, *recent_menu;
  Ihandle *file_menu, *file_submenu, *item_new, *item_open, *item_close, 
          *item_merge, *import_menu, *import_submenu, *item_import_gedcom, 
          *export_menu, *export_submenu, *item_export_gedcom55, 
          *item_export_gedcom551, *item_exit;
  Ihandle *help_menu, *help_submenu, *item_help, *item_setbrowser, *item_about;
  
  item_new = IupItem("&New...\tCtrl+N", NULL); /* Dunno if it's a good idea to have a shortcut for New */
  
  item_open = IupItem("&Open...\tCtrl+O", NULL);
  
  item_close = IupItem("&Close", NULL);
  IupSetAttribute(item_close, "NAME", "ITEM_CLOSE");
  
  item_merge = IupItem("&Merge...", NULL);
  
  item_import_gedcom = IupItem("Import &GEDCOM File...", NULL);
  
  item_export_gedcom55 = IupItem("Export GEDCOM &5.5 File...", NULL);
  
  item_export_gedcom551 = IupItem("Export GEDCOM 5.5.&1 File...", NULL);
  
  item_exit = IupItem("E&xit", NULL);
  
  item_help = IupItem("Help...\tF1", NULL);
  IupSetAttribute(item_exit, "NAME", "ITEM_EXIT");
  IupSetCallback(item_exit, "ACTION", (Icallback) item_exit_action_cb);
  
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
  
  help_menu = IupMenu(
                      item_help,
                      item_setbrowser,
                      item_about,
                      NULL);
  
  file_submenu = IupSubmenu("&File", file_menu);
  help_submenu = IupSubmenu("&Help", help_menu);
  
  menu = IupMenu(
                 file_submenu,
                 help_submenu,
                 NULL);
  
  /* Initialise variables from the Config file */
  IupConfigRecentInit(config, recent_menu, config_recent_cb, 10);
  /* TODO: Write config_recent_cb! */
  
  return menu;
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
  
  relationship_list = IupList(NULL);
  IupSetAttribute(relationship_list, "NAME", "RELATIONSHIPLIST");
  IupSetAttribute(relationship_list, "1", NULL);
  IupSetAttribute(relationship_list, "DROPDOWN", "NO");
  IupSetAttribute(relationship_list, "SORT", "NO");
  IupSetAttribute(relationship_list, "ACTIVE", "NO");
  
  source_list = IupList(NULL);
  IupSetAttribute(source_list, "NAME", "SOURCELIST");
  IupSetAttribute(source_list, "1", NULL);
  IupSetAttribute(source_list, "DROPDOWN", "NO");
  IupSetAttribute(source_list, "SORT", "NO");
  IupSetAttribute(source_list, "ACTIVE", "NO");
  
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
  
  /* TODO: Create Toolbar and Statusbar! */
  
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
  Ihandle *config, dlg;
  
  IupOpen(&argc, &argv);
  config = IupConfig();
  IupSetAttribute(config, "APP_NAME", PROG_NAME);
  IupSetAttribute(config, "APP_CONFIG", "NO");
  SafeIupConfigLoad(config);
  
  dlg = create_mainwindow(config);
  
  IupMainLoop();
  
  IupClose();
  return EXIT_SUCCESS;
}
