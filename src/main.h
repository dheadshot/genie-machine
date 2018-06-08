#ifndef __MAIN_H__

#define __MAIN_H__ 1


#include <iup/iup.h>
#include <iup/iup_config.h>

/* --- Helper Functions --- */
const char *str_fileext(const char *filename);
const char *str_filename(const char *filename);
long str_filepath(char *outpath, const char *infilename);
int selectfile(Ihandle *parentdlg, int isopen);
int populatemainlists(Ihandle *ih);
void donewfile(Ihandle *ih);
int dofileisopenactivation(Ihandle *ih);
int doclosefile(Ihandle *ih);
void toggle_bar_visibility(Ihandle *item, Ihandle *ih);

/* --- Callback Functions --- */
int item_exit_action_cb(Ihandle *item_exit);
int item_new_action_cb(Ihandle *item_new);
int config_recent_cb(Ihandle *ih);
int item_open_action_cb(Ihandle *item_open);
int item_showtoolbar_action_cb(Ihandle *item_showtoolbar);
int item_showstatusbar_action_cb(Ihandle *item_showstatusbar);
int item_setbrowser_action_cb(Ihandle *item_setbrowser);
int item_newperson_action_cb(Ihandle *item_newperson);
int file_menu_open_cb(Ihandle *ih);
int person_menu_open_cb(Ihandle *ih);

/* --- Main Functions --- */
Ihandle *create_mainwindow_menu(Ihandle *config);
Ihandle *create_toolbar(Ihandle *config);
Ihandle *create_statusbar(Ihandle *config);
Ihandle *create_mainwindow(Ihandle *config);
int main (int argc, char *argv[]);


#endif