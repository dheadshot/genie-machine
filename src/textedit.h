#ifndef __TEXTEDIT_H__
#define __TEXTEDIT_H__ 1

#include <iup/iup.h>
#include <iup/iup_config.h>

void tedit_toggle_bar_visibility(Ihandle *item, Ihandle *ih);

int tedit_cancelbtn_action_cb(Ihandle *cancelbtn);
int tedit_okbtn_action_cb(Ihandle *okbtn);
int tedit_maintext_valuechanged_cb(Ihandle *ih);
int tedit_edit_menu_open_cb(Ihandle *ih);
int tedit_maintext_caret_cb(Ihandle *ih, int lin, int col);
int tedit_item_revert_action_cb(Ihandle *item_revert);
int tedit_item_copy_action_cb(Ihandle *item_copy);
int tedit_item_paste_action_cb(Ihandle *item_paste);
int tedit_item_cut_action_cb(Ihandle *item_cut);
int tedit_item_delete_action_cb(Ihandle *item_delete);
int tedit_item_select_all_action_cb(Ihandle *item_select_all);
int tedit_item_toolbar_action_cb(Ihandle *item_toolbar);
int tedit_item_statusbar_action_cb(Ihandle *item_statusbar);
int tedit_item_help_action_cb(Ihandle *ih);
int tedit_item_find_action_cb(Ihandle *item_find);
int tedit_item_goto_action_cb(Ihandle *item_goto);
int tedit_insertutf8_action_cb(Ihandle *item_insertutf8);
Ihandle *tedit_makedlg(Ihandle *config, const char *caption);
int dotedit(Ihandle *config, const char *origintext, char **returntextvar, const char *caption, Ihandle *parentdlg, const char *prevwindowlist);

#endif