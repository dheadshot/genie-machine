#ifndef __WINDOWLIST_H__
#define __WINDOWLIST_H__ 1

int initwindowlist(char **windowlistvar);
int freewindowlist(char **windowlistvar);
int addwindowtolist(char **windowlistvar, const char *windowname);
int duplicatewindowlist(char **newwindowlistvar, const char *windowlist);
int findwindowinlist(const char *windowlist, const char *windowname); /* Returns the window's position (index 0) in the list, or -1 if it isn't there. */

#endif