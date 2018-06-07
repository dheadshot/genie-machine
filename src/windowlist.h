#ifndef __WINDOWLIST_H__
#define __WINDOWLIST_H__ 1

int initwindowlist(char **windowlistvar);
int freewindowlist(char **windowlistvar);
int addwindowtolist(char **windowlistvar, char *windowname);
int duplicatewindowlist(char **newwindowlistvar, char *windowlist);
int findwindowinlist(char *windowlist, char *windowname); /* Returns the window's position (index 0) in the list, or -1 if it isn't there. */

#endif