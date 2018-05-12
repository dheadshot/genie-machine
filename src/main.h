#ifndef __MAIN_H__

#define __MAIN_H__ 1


#include <iup/iup.h>
#include <iup/iup_config.h>

/* --- Helper Functions --- */
int selectfile(Ihandle *parentdlg, int isopen);
int populatemainlists(Ihandle *ih);
void donewfile(Ihandle *ih);

/* --- Callback Functions --- */
int item_exit_action_cb(Ihandle *item_exit);
int item_new_action_cb(Ihandle *item_new);
int config_recent_cb(Ihandle *ih);
int file_menu_open_cb(Ihandle *ih);

/* --- Main Functions --- */
Ihandle *create_mainwindow_menu(Ihandle *config);
Ihandle *create_toolbar(Ihandle *config);
Ihandle *create_statusbar(Ihandle *config);
Ihandle *create_mainwindow(Ihandle *config);
int main (int argc, char *argv[]);


#endif