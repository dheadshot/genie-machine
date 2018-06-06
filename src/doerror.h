#ifndef __DOERROR_H__
#define __DOERROR_H__ 1

#include <iup/iup.h>

void show_error(const char *message, int is_error, const char *title, Ihandle *parentdlg); /* Title and parentdlg can be NULL */

#endif