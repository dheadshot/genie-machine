#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "dbaccess.h"


int dbisopen = 0;
sqlite3 *db = NULL;
unsigned long lastdberr = 0;


unsigned long getlastdberr()
{
  return lastdberr;
}

int opendb(char *dbfilename, int newfile)
{
  int retcode;
  if (dbisopen) return -1;
  if (newfile)
  {
    retcode = sqlite3_open_v2(dbfilename, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  }
  else
  {
    retcode = sqlite3_open_v2(dbfilename, &db, SQLITE_OPEN_READWRITE, NULL);
  }
  if (retcode != SQLITE_OK)
  {
    lastdberr = retcode;
    sqlite3_close(db);
    return 0;
  }
  dbopen = 1;
  return 1;
}
