#ifndef __DBACCESS_H__

#define __DBACCESS_H__ 1


int setdberrtext(const char *dberrtext);
void unsetdberrtext();
unsigned long getlastdberr();
int opendb(char *dbfilename, int newfile);
int closedb();
int initnewdb();

#ifndef __DBACCESS_DECLARE__
extern char *lastdberrtext;
extern unsigned long lastdberl;
#endif


#endif