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
ntsa getdb_mainpersonlist(int maxname);
ntsa getdb_mainrelationshiplist(int maxname);
ntsa getdb_mainsourcelist(int maxdesc);
sqlite3_int64 addnewperson(const char *bio, const char *ethnicity, 
                           const char *sex, const char *gender, 
                           const char *notes, int userelkidid, 
                           sqlite3_int64 relkidid, int isadopted,
                           sqlite3_int64 bdateid, const char *bplace, 
                           int usebaddr, sqlite3_int64 baddrid, 
                           int isdead, sqlite3_int64 ddateid, 
                           const char *dplace, int usedaddr,
                           sqlite3_int64 daddrid, sqlite3_int64 dageid, 
                           int usesource, sqlite3_int64 sourceid);
int modifynewperson(sqlite3_int64 personid, const char *bio, 
                    const char *ethnicity, const char *sex, 
                    const char *gender, const char *notes, 
                    int userelkidid, sqlite3_int64 relkidid, 
                    int isadopted,sqlite3_int64 bdateid, 
                    const char *bplace, int usebaddr, 
                    sqlite3_int64 baddrid, int isdead, 
                    sqlite3_int64 ddateid, const char *dplace, 
                    int usedaddr, sqlite3_int64 daddrid, 
                    sqlite3_int64 dageid, int usesource, 
                    sqlite3_int64 sourceid);


#ifndef __DBACCESS_DECLARE__
extern char *lastdberrtext;
extern unsigned long lastdberl;
#endif


#endif