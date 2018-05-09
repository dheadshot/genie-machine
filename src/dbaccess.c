#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "proginfo.h"
#include "dbdata.h"

#define __DBACCESS_DECLARE__ 1
#include "dbaccess.h"


int dbisopen = 0;
sqlite3 *db = NULL;
unsigned long lastdberr = 0;
unsigned long lastdberl = 0;
char *lastdberrtext = NULL;


int setdberrtext(const char *dberrtext)
{
  if (!dberrtext) return 0;
  if (lastdberrtext) free(lastdberrtext);
  lastdberrtext = NULL;
  lastdberrtext = (char *) malloc(sizeof(char) * (1+strlen(dberrtext)));
  if (!lastdberrtext) return 0;
  strcpy(lastdberrtext, dberrtext);
  return 1;
}

void unsetdberrtext()
{
  if (lastdberrtext) free(lastdberrtext);
  lastdberrtext = NULL;
}

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
    unsetdberrtext();
    lastdberr = retcode;
    sqlite3_close(db);
    return 0;
  }
  dbopen = 1;
  return 1;
}

int closedb()
{
  unsetdberrtext();
  if (!dbisopen) return -1;
  int rc = sqlite3_close(db);
  if (rc == SQLITE_OK || rc == SQLITE_DONE) return 1;
  lastdberr = rc;
  return 0;
}

int initnewdb()
{
  /* Initialise a newly created database with the correct tables and initial values */
  lastdberr = 0;
  unsetdberrtext();
  if (!dbopen) return 0;
  int rc = 0;
  unsigned int rl = 0;
  char *errmsg;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_pragma_foreign_keys_off, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  /* Create Tables */
  rl++;
  rc = sqlite3_exec(db, newdb_sql_begin, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_person, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_date, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_datetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_calendartype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_age, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_agetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_personname, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_physdesc, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_beliefs, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_belieftype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_nationality, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_affiltype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_adoptfoster, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_adoptpersontype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_adopttype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_relationship, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_reltype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_relendtype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_job, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_address, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_livingaddr, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_workingaddr, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_othercontact, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_contacttype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_educachieve, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_event, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_source, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_sourcetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_multimedia, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_inmultimedia, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_iso6393, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_metainfo, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_commit, NULL, NULL, &errmsg); /* This could be the kicker */
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  /* If rc was SQLITE_BUSY then retry later? */
  
  /* Insert Data */
  rl++;
  rc = sqlite3_exec(db, newdb_sql_begin, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_datetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_calendartype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_agetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_belieftype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_affiltype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_adoptpersontype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_adopttype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_reltype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_relendtype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_contacttype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_sourcetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_metainfo, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_iso6393, NULL, NULL, &errmsg); /* Another possible kicker */
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_commit, NULL, NULL, &errmsg); /* Yet another possible kicker */
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  /* If rc was SQLITE_BUSY then retry later? */
  
  /* Update Statements */
  sqlite3_stmt *progversql = NULL;
  
  rl++;
  rc = sqlite3_prepare_v2(db, newdb_sql_update_metainfo_version, -1, &progversql, NULL);
  if (rc != SQLITE_OK) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_bind_text(progversql, 1, PROG_VER, -1, SQLITE_TRANSIENT);
  if (rc != SQLITE_OK) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_step(progversql);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  rl++;
  rc = sqlite3_finalize(progversql);
  if (rc != SQLITE_OK) goto ErrInitNewDB;
  progversql = NULL;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_update_metainfo_last_edit, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  /* Should the Foreign Keys Pragma be enabled at this point? */
  
  /* Vacuum DB */
  rl++;
  rc = sqlite3_exec(db, newdb_sql_vacuum, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  
  return 1;
  
ErrInitNewDB:
  setdberrtext(errmsg);
  lastdberr = rc;
  lastdberl = rl;
  return 0;
}
