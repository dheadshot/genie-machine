#ifndef __DOERROR_H__
#define __DOERROR_H__ 1

#include <iup/iup.h>

void show_error(const char *message, int is_error, const char *title, Ihandle *parentdlg); /* Title and parentdlg can be NULL */
int error_filedoesnotexist(const char *filename, Ihandle *parentdlg); /* Parentdlg can be NULL */
int error_openingdb(const char *filename, const unsigned long errnum, Ihandle *parentdlg); /* Parentdlg can be NULL */
int error_accessingdb(const unsigned long errnum, const unsigned long errline, const char *errtext, Ihandle *parentdlg); /* Parentdlg can be NULL */
int error_initdb(const unsigned long errnum, const unsigned long errline, const char *errtext, Ihandle *parentdlg); /* Parentdlg can be NULL */
int error_closingdb(const unsigned long errnum, Ihandle *parentdlg); /* Parentdlg can be NULL */

#endif