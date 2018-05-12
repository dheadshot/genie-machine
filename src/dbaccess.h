#ifndef __DBACCESS_H__

#define __DBACCESS_H__ 1


#include <sqlite3.h>
#include "types.h"

int setdberrtext(const char *dberrtext);
void unsetdberrtext();
unsigned long getlastdberr();
char *formatname(const char *familyname, const char *givenname, 
                 const char *patronymic, const char *othernames, 
                 const char *titleprefix, char *titlesuffix, 
                 const char *titleinterpart, const char *nameformat);
int opendb(char *dbfilename, int newfile);
int closedb();
int initnewdb();
ntsa getdb_mainpersonlist();

#ifndef __DBACCESS_DECLARE__
extern char *lastdberrtext;
extern unsigned long lastdberl;
#endif


#endif