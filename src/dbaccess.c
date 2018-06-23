#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <ctype.h>

#include "utf8.h"
#include "proginfo.h"
#include "dbdata.h"
#include "types.h"

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

char *formatname(const char *familyname, const char *givenname, 
                 const char *patronymic, const char *othernames, 
                 const char *titleprefix, const char *titlesuffix, 
                 const char *titleinterpart, const char *nameformat)
{
  unsetdberrtext();
  if (nameformat == NULL) return NULL;
  unsigned long tlen = 0;
  if (familyname) tlen += strlen(familyname);
  if (givenname) tlen += strlen(givenname);
  if (patronymic) tlen += strlen(patronymic);
  if (othernames) tlen += strlen(othernames);
  if (titleprefix) tlen += strlen(titleprefix);
  if (titlesuffix) tlen += strlen(titlesuffix);
  if (titleinterpart) tlen += strlen(titleinterpart);
  if (tlen == 0) return NULL;
  tlen += strlen(nameformat);
  tlen += 10;
  
  int i;
  char *ans = (char *) malloc(sizeof(char)*tlen);
  if (ans == NULL)
  {
    setdberrtext("Out of Memory forming name!");
    return NULL;
  }
  ans[0] = 0;
  if (str_comparet_nt(nameformat,"?",1))
  {
    strcpy(ans, "?");
    if (titleprefix) strcpy(ans, titleprefix);
    strcpy(ans, " ");
    if (givenname) strcpy(ans, givenname);
    strcpy(ans, " ");
    if (othernames) strcpy(ans, othernames);
    strcpy(ans, " ");
    if (patronymic) strcpy(ans, patronymic);
    strcpy(ans, " ");
    if (titleinterpart) strcpy(ans, titleinterpart);
    strcpy(ans, " ");
    if (familyname) strcpy(ans, familyname);
    strcpy(ans, " ");
    if (titlesuffix) strcpy(ans, titlesuffix);
    strcpy(ans, "?");
  }
  else
  {
    for (i = 0; nameformat[i] != 0; i++)
    {
      switch (nameformat[i])
      {
        case 'G':
          if (givenname) strcat(ans, givenname);
          else strcat(ans, "?");
        break;
        
        case 'F':
          if (familyname) strcat(ans, familyname);
          else strcat(ans, "?");
        break;
        
        case 'P':
          if (patronymic) strcat(ans, patronymic);
          else strcat(ans, "?");
        break;
        
        case 'O':
          if (othernames) strcat(ans, othernames);
          else strcat(ans, "?");
        break;
        
        case 'T':
          if (titleprefix) strcat(ans, titleprefix);
          else strcat(ans, "?");
        break;
        
        case 'S':
          if (titlesuffix) strcat(ans, titlesuffix);
          else strcat(ans, "?");
        break;
        
        case 'I':
          if (titleinterpart) strcat(ans, titleinterpart);
          else strcat(ans, "?");
        break;
        
        default:
        {
          unsigned long l;
          l = strlen(ans)+1;
          ans[l] = nameformat[i];
          ans[l+1] = 0;
        }
        break;
      }
    }
  }
  return ans;
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
    	fprintf(stderr,"---DB Opening Error!\n");
    sqlite3_close(db);
    return 0;
  }
  dbisopen = 1;
  return 1;
}

int closedb()
{
  unsetdberrtext();
  if (!dbisopen) return -1;
  int rc = sqlite3_close(db);
  if (rc == SQLITE_OK || rc == SQLITE_DONE)
  {
    dbisopen = 0;
    return 1;
  }
  lastdberr = rc;
  return 0;
}

int initnewdb()
{
  /* Initialise a newly created database with the correct tables and initial values */
  	fprintf(stderr,"---Initialising DB---\n");
  lastdberr = 0;
  lastdberl = 0;
  unsetdberrtext();
  if (!dbisopen) return 0;
  int rc = 0;
  unsigned int rl = 0;
  char *errmsg = NULL;
  
  	fprintf(stderr,"---%u: Starting---\n",rl);
  rl++;
  	fprintf(stderr,"---%u: Running %p---\n",rl,newdb_sql_pragma_foreign_keys_off);
  	fprintf(stderr,"---%u: Running '%s'---\n",rl,newdb_sql_pragma_foreign_keys_off);
  rc = sqlite3_exec(db, newdb_sql_pragma_foreign_keys_off, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: FK Off---\n",rl);
  
  /* Create Tables */
  rl++;
  rc = sqlite3_exec(db, newdb_sql_begin, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Begin---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_person, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Person---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_date, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Date---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_datetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create DateType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_calendartype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create CalendarType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_age, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Age---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_agetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create AgeType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_personname, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create PersonName---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_physdesc, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create PhysDesc---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_beliefs, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Beliefs---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_belieftype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create BeliefType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_nationality, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Nationality---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_affiltype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create AffilType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_adoptfoster, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create AdoptFoster---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_adoptpersontype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create AdoptPersonType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_adopttype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create AdoptType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_relationship, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Relationship---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_reltype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create RelType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_relendtype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create RelEndType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_job, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Job---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_address, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Address---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_livingaddr, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create LivingAddr---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_workingaddr, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create WorkingAddr---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_othercontact, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create OtherContact---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_contacttype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create ContactType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_educachieve, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create EducAchieve---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_event, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Event---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_source, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Source---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_sourcetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create SourceType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_multimedia, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create Multimedia---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_inmultimedia, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create InMultimedia---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_iso6393, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create ISO6393---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_create_metainfo, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Create MetaInfo---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_commit, NULL, NULL, &errmsg); /* This could be the kicker */
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  /* If rc was SQLITE_BUSY then retry later? */
  	fprintf(stderr,"---%u: Commit---\n",rl);
  
  /* Insert Data */
  rl++;
  rc = sqlite3_exec(db, newdb_sql_begin, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Begin---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_datetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert DateType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_calendartype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert CalendarType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_agetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert AgeType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_belieftype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert BeliefType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_affiltype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert AffilType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_adoptpersontype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert AdoptPerson---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_adopttype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert AdoptType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_reltype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert RelType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_relendtype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert RelEndType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_contacttype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert ContactType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_sourcetype, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert SourceType---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_metainfo, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert MetaInfo---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_insert_iso6393, NULL, NULL, &errmsg); /* Another possible kicker */
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Insert ISO6393---\n",rl);
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_commit, NULL, NULL, &errmsg); /* Yet another possible kicker */
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  /* If rc was SQLITE_BUSY then retry later? */
  	fprintf(stderr,"---%u: Commit---\n",rl);
  
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
  	fprintf(stderr,"---%u: Update MetaInfo Version---\n",rl);
  
  rl++;
  rc = sqlite3_finalize(progversql);
  if (rc != SQLITE_OK) goto ErrInitNewDB;
  progversql = NULL;
  
  rl++;
  rc = sqlite3_exec(db, newdb_sql_update_metainfo_last_edit, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Update MetaInfo Last_Edit---\n",rl);
  
  /* Should the Foreign Keys Pragma be enabled at this point? */
  
  /* Vacuum DB */
  rl++;
  rc = sqlite3_exec(db, newdb_sql_vacuum, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) goto ErrInitNewDB;
  	fprintf(stderr,"---%u: Vacuum---\n",rl);
  
  return 1;
  
ErrInitNewDB:
  	fprintf(stderr,"---%u: Err: %s!---\n",rl, errmsg);
  setdberrtext(errmsg);
  lastdberr = rc;
  lastdberl = rl;
  if (errmsg) sqlite3_free(errmsg);
  return 0;
}

ntsa getdb_mainpersonlist(int maxname)
{
  lastdberr = 0;
  lastdberl = 0;
  unsetdberrtext();
  if (!dbisopen) return NULL;
  
  char *errmsg = NULL;
  ntsa thelist;
  int rc, rl = 0;
  sqlite3_stmt *countstmt, *tblstmt;
  sqlite3_int64 tblcount;
  
  rl++;
  rc = sqlite3_prepare_v2(db, "SELECT Count(PersonID) FROM Person;", 36, &countstmt, NULL);
  if (rc != SQLITE_OK) goto ErrGetDB_MainPersonList;
  
  rl++;
  rc = sqlite3_step(countstmt);
  if (rc != SQLITE_ROW && rc != SQLITE_DONE && rc != SQLITE_OK) goto ErrGetDB_MainPersonList;
  
  rl++;
  tblcount = sqlite3_column_int64(countstmt,0);
  
  thelist = (ntsa) malloc(sizeof(char *) * (tblcount + 1));
  if (thelist == NULL)
  {
    setdberrtext("Out of Memory loading data from Database!");
    rc = 0;
    goto ErrGetDB_MainPersonList;
  }
  thelist[tblcount] = NULL;
  
  rl++;
  rc = sqlite3_finalize(countstmt);
  if (rc != SQLITE_OK)
  {
    goto ErrGetDB_MainPersonList;
  }
  
  if (tblcount == 0) return thelist;
  
  rl++;
  rc = sqlite3_prepare_v2(db, sql_select_mainpersonlist, -1, &tblstmt, NULL);
  if (rc != SQLITE_OK)
  {
    free(thelist);
    goto ErrGetDB_MainPersonList;
  }
  
  rl++;
  rc = SQLITE_ROW;
  sqlite3_int64 personid, j = 0;
  const char *familyname, *givenname, *patronymic, *othernames, 
             *titleprefix, *titlesuffix, *titleinterpart, *nameformat, 
             *datetype, *year1, *year2;
  char *fullname = NULL, *byear = NULL, *personidtxt = NULL;
  while (rc == SQLITE_ROW)
  {
    rc = sqlite3_step(tblstmt);
    if (rc != SQLITE_ROW && rc != SQLITE_DONE && rc != SQLITE_OK) goto ErrGetDB_MainPersonList_Freethelist;
    
    if (rc == SQLITE_ROW && j < tblcount)
    {
      familyname = sqlite3_column_text(tblstmt, 0);
      givenname = sqlite3_column_text(tblstmt, 1);
      patronymic = sqlite3_column_text(tblstmt, 2);
      othernames = sqlite3_column_text(tblstmt, 3);
      titleprefix = sqlite3_column_text(tblstmt, 4);
      titlesuffix = sqlite3_column_text(tblstmt, 5);
      titleinterpart = sqlite3_column_text(tblstmt, 6);
      nameformat = sqlite3_column_text(tblstmt, 7);
      personid = sqlite3_column_int64(tblstmt, 8);
      datetype = sqlite3_column_text(tblstmt, 9);
      year1 = sqlite3_column_text(tblstmt, 10);
      year2 = sqlite3_column_text(tblstmt, 11);
      
      if (nameformat)
      {
        fullname = formatname(familyname, givenname, patronymic, othernames, 
                              titleprefix, titlesuffix, titleinterpart, 
                              nameformat);
      }
      else
      {
        fullname = (char *) malloc(sizeof(char));
        if (fullname) fullname[0] = 0;
      }
      if (fullname == NULL)
      {
        goto ErrGetDB_MainPersonList_Freethelist;
      }
      if (maxname > 0 && strlen(fullname) > maxname) fullname[maxname] = 0;
      
      byear = (char *) malloc(sizeof(char)*(1+sstrlen(year1)+sstrlen(year2)+2));
      if (byear == NULL)
      {
        free(fullname);
        goto ErrGetDB_MainPersonList_Freethelist;
      }
      strcpy(byear, "b");
      if (str_comparet_nt(datetype,"BET",0))
      {
        if (year1) strcat(byear, year1); else strcat(byear, "????");
        strcat(byear, "-");
        if (year2) strcat(byear, year2); else strcat(byear, "????");
      }
      else
      {
        if (str_comparet_nt(datetype,"ABT",0)) strcat(byear,"~");
        else if (str_comparet_nt(datetype,"BEF",0)) strcat(byear,"<");
        else if (str_comparet_nt(datetype,"AFT",0)) strcat(byear,">");
        if (year1) strcat(byear, year1); else strcat(byear, "????");
      }
      
      personidtxt = sqlite3_mprintf(" %lld)", personid);
      if (personidtxt == NULL)
      {
        free(fullname);
        free(byear);
        goto ErrGetDB_MainPersonList_Freethelist;
      }
      
      thelist[j] = (char *) malloc(sizeof(char)*(3+strlen(fullname)+strlen(byear)+strlen(personidtxt)));
      if (thelist[j] == NULL)
      {
        free(fullname);
        free(byear);
        sqlite3_free(personidtxt);
        goto ErrGetDB_MainPersonList_Freethelist;
      }
      strcpy(thelist[j], fullname);
      strcat(thelist[j], " (");
      strcat(thelist[j], byear);
      strcat(thelist[j], personidtxt);
      
      free(fullname);
      fullname = NULL;
      free(byear);
      byear = NULL;
      sqlite3_free(personidtxt);
      personidtxt = NULL;
      
      j++;
    }
  }
  
  rl++;
  rc = sqlite3_finalize(tblstmt);
  if (rc != SQLITE_OK) goto ErrGetDB_MainPersonList_Freethelist;
  
  return thelist;
  
  
ErrGetDB_MainPersonList_Freethelist:
  j += 0;
  sqlite3_int64 k;
  if (j) for (k = 0; k<j; k++) free(thelist[k]);
  free(thelist);
  
ErrGetDB_MainPersonList:
  if (errmsg)
  {
    setdberrtext(errmsg);
    sqlite3_free(errmsg);
  }
  lastdberr = rc;
  lastdberl = rl;
  return NULL;
}

ntsa getdb_mainrelationshiplist(int maxname)
{
  lastdberr = 0;
  lastdberl = 0;
  unsetdberrtext();
  if (!dbisopen) return NULL;
  
  char *errmsg = NULL;
  ntsa thelist;
  int rc, rl = 0;
  sqlite3_stmt *countstmt, *tblstmt;
  sqlite3_int64 tblcount;
  
  rl++;
  rc = sqlite3_prepare_v2(db, "SELECT Count(RelID) FROM Relationship;", 39, &countstmt, NULL);
  if (rc != SQLITE_OK) goto ErrGetDB_MainRelationshipList;
  
  rl++;
  rc = sqlite3_step(countstmt);
  if (rc != SQLITE_ROW && rc != SQLITE_DONE && rc != SQLITE_OK) goto ErrGetDB_MainRelationshipList;
  
  rl++;
  tblcount = sqlite3_column_int64(countstmt,0);
  
  thelist = (ntsa) malloc(sizeof(char *) * (tblcount + 1));
  if (thelist == NULL)
  {
    setdberrtext("Out of Memory loading data from Database!");
    rc = 0;
    goto ErrGetDB_MainRelationshipList;
  }
  thelist[tblcount] = NULL;
  
  rl++;
  rc = sqlite3_finalize(countstmt);
  if (rc != SQLITE_OK)
  {
    goto ErrGetDB_MainRelationshipList;
  }
  
  if (tblcount == 0) return thelist;
  
  rl++;
  rc = sqlite3_prepare_v2(db, sql_select_mainrelationshiplist, -1, &tblstmt, NULL);
  if (rc != SQLITE_OK)
  {
    free(thelist);
    goto ErrGetDB_MainRelationshipList;
  }
  
  rl++;
  rc = SQLITE_ROW;
  sqlite3_int64 j=0, p1personid, p2personid, relid;
  const char *p1familyname, *p1givenname, *p1patronymic, 
             *p1othernames, *p1titleprefix, *p1titlesuffix, 
             *p1titleinterpart, *p1nameformat, *p2familyname, 
             *p2givenname, *p2patronymic, *p2othernames, 
             *p2titleprefix, *p2titlesuffix, *p2titleinterpart, 
             *p2nameformat, *reltype, *d1datetype, *d1year1, *d1year2, 
             *d2datetype, *d2year1, *d2year2;
  char *p1fullname, *p2fullname, *p1extname, *p2extname, *sdate, 
       *edate, *daterange, *relidtxt;
  
  while (rc == SQLITE_ROW)
  {
    rc = sqlite3_step(tblstmt);
    if (rc != SQLITE_ROW && rc != SQLITE_DONE && rc != SQLITE_OK) goto ErrGetDB_MainRelationshipList_Freethelist;
    
    if (rc == SQLITE_ROW && j < tblcount)
    {
      p1familyname = sqlite3_column_text(tblstmt, 0);
      p1givenname = sqlite3_column_text(tblstmt, 1);
      p1patronymic = sqlite3_column_text(tblstmt, 2);
      p1othernames = sqlite3_column_text(tblstmt, 3);
      p1titleprefix = sqlite3_column_text(tblstmt, 4);
      p1titlesuffix = sqlite3_column_text(tblstmt, 5);
      p1titleinterpart = sqlite3_column_text(tblstmt, 6);
      p1nameformat = sqlite3_column_text(tblstmt, 7);
      p1personid = sqlite3_column_int64(tblstmt, 8);
      p2familyname = sqlite3_column_text(tblstmt, 9);
      p2givenname = sqlite3_column_text(tblstmt, 10);
      p2patronymic = sqlite3_column_text(tblstmt, 11);
      p2othernames = sqlite3_column_text(tblstmt, 12);
      p2titleprefix = sqlite3_column_text(tblstmt, 13);
      p2titlesuffix = sqlite3_column_text(tblstmt, 14);
      p2titleinterpart = sqlite3_column_text(tblstmt, 15);
      p2nameformat = sqlite3_column_text(tblstmt, 16);
      p2personid = sqlite3_column_int64(tblstmt, 17);
      reltype = sqlite3_column_text(tblstmt, 18);
      d1datetype = sqlite3_column_text(tblstmt, 19);
      d1year1 = sqlite3_column_text(tblstmt, 20);
      d1year2 = sqlite3_column_text(tblstmt, 21);
      d2datetype = sqlite3_column_text(tblstmt, 22);
      d2year1 = sqlite3_column_text(tblstmt, 23);
      d2year2 = sqlite3_column_text(tblstmt, 24);
      relid = sqlite3_column_int64(tblstmt, 25);
      
      if (p1nameformat)
      {
        p1fullname = formatname(p1familyname, p1givenname, p1patronymic, 
                                p1othernames, p1titleprefix, p1titlesuffix, 
                                p1titleinterpart, p1nameformat);
      }
      else
      {
          p1fullname = (char *) malloc(sizeof(char));
          if (p1fullname) p1fullname[0] = 0;
      }
      if (p1fullname == NULL)
      {
        goto ErrGetDB_MainRelationshipList_Freethelist;
      }
      if (maxname > 0 && strlen(p1fullname) > maxname) p1fullname[maxname] = 0;
      if (p2nameformat || p2personid)
      {
        if (p2nameformat)
        {
          p2fullname = formatname(p2familyname, p2givenname, p2patronymic, 
                                  p2othernames, p2titleprefix, p2titlesuffix, 
                                  p2titleinterpart, p2nameformat);
        }
        else
        {
          p2fullname = (char *) malloc(sizeof(char));
          if (p2fullname) p2fullname[0] = 0;
          else setdberrtext("Out of Memory!");
        }
        if (p2fullname == NULL)
        {
          free(p1fullname);
          goto ErrGetDB_MainRelationshipList_Freethelist;
        }
      }
      else
      {
        p2fullname = (char *) malloc(sizeof(char)*2);
        if (p2fullname == NULL)
        {
          free(p1fullname);
          setdberrtext("Out of Memory!");
          goto ErrGetDB_MainRelationshipList_Freethelist;
        }
        strcpy(p2fullname,"?");
      }
      if (maxname > 0 && strlen(p2fullname) > maxname) p2fullname[maxname] = 0;
      
      /* p1extname = (char *) malloc(sizeof(char)*(51+strlen(p1fullname)));
      if (!p1extname)
      {
        free(p1fullname);
        free(p2fullname);
        goto ErrGetDB_MainRelationshipList_Freethelist;
      }
      p2extname = (char *) malloc(sizeof(char)*(51+strlen(p2fullname)));
      if (!p2extname)
      {
        free(p1extname);
        free(p1fullname);
        free(p2fullname);
        goto ErrGetDB_MainRelationshipList_Freethelist;
      }
      */
      p1extname = sqlite3_mprintf("%s (%lld)", p1fullname, p1personid);
      if (!p1extname)
      {
        free(p1fullname);
        free(p2fullname);
        setdberrtext("Out of Memory!");
        goto ErrGetDB_MainRelationshipList_Freethelist;
      }
      if (p2personid)
      {
        p2extname = sqlite3_mprintf("%s (%lld)", p2fullname, p2personid);
      }
      else
      {
        p2extname = sqlite3_mprintf("?");
      }
      if (!p2extname)
      {
        sqlite3_free(p1extname);
        free(p1fullname);
        free(p2fullname);
        setdberrtext("Out of Memory!");
        goto ErrGetDB_MainRelationshipList_Freethelist;
      }
      
      free(p1fullname);
      p1fullname = NULL;
      free(p2fullname);
      p2fullname = NULL;
      
      sdate = (char *) malloc(sizeof(char)*(sstrlen(d1year1)+sstrlen(d1year2)+11));
      if (!sdate)
      {
        sqlite3_free(p1extname);
        sqlite3_free(p2extname);
        setdberrtext("Out of Memory!");
        goto ErrGetDB_MainRelationshipList_Freethelist;
      }
      if (str_comparet_nt(d1datetype,"BET",0))
      {
        strcpy(sdate,"[");
        if (d1year1) strcat(sdate, d1year1); else strcat(sdate, "????");
        strcpy(sdate,"-");
        if (d1year2) strcat(sdate, d1year2); else strcat(sdate, "????");
        strcat(sdate,"]");
      }
      else
      {
        if (str_comparet_nt(d1datetype,"ABT",0)) strcat(sdate,"~");
        else if (str_comparet_nt(d1datetype,"BEF",0)) strcat(sdate,"<");
        else if (str_comparet_nt(d1datetype,"AFT",0)) strcat(sdate,">");
        if (d1year1) strcat(sdate, d1year1); else strcat(sdate, "????");
      }
      
      if (d2datetype)
      {
        edate = (char *) malloc(sizeof(char)*(sstrlen(d2year1)+sstrlen(d2year2)+11));
        if (!edate)
        {
          sqlite3_free(p1extname);
          sqlite3_free(p2extname);
          free(sdate);
          setdberrtext("Out of Memory!");
          goto ErrGetDB_MainRelationshipList_Freethelist;
        }
        if (str_comparet_nt(d2datetype,"BET",0))
        {
          strcpy(edate,"[");
          if (d2year1) strcat(edate, d2year1); else strcat(edate, "????");
          strcpy(edate,"-");
          if (d2year2) strcat(edate, d2year2); else strcat(edate, "????");
          strcat(edate,"]");
        }
        else
        {
          if (str_comparet_nt(d2datetype,"ABT",0)) strcat(edate,"~");
          else if (str_comparet_nt(d2datetype,"BEF",0)) strcat(edate,"<");
          else if (str_comparet_nt(d2datetype,"AFT",0)) strcat(edate,">");
          if (d2year1) strcat(edate, d2year1); else strcat(edate, "????");
        }
      }
      else
      {
        edate = (char *) malloc(sizeof(char));
        if (!edate)
        {
          sqlite3_free(p1extname);
          sqlite3_free(p2extname);
          free(sdate);
          setdberrtext("Out of Memory!");
          goto ErrGetDB_MainRelationshipList_Freethelist;
        }
        edate[0] = 0;
      }
      
      daterange = (char *) malloc(sizeof(char)*(strlen(sdate)+strlen(edate)+5));
      if (!daterange)
      {
        sqlite3_free(p1extname);
        sqlite3_free(p2extname);
        free(sdate);
        free(edate);
        setdberrtext("Out of Memory!");
        goto ErrGetDB_MainRelationshipList_Freethelist;
      }
      strcpy(daterange, sdate);
      strcat(daterange, " - ");
      strcat(daterange, edate);
      free(sdate);
      sdate = NULL;
      free(edate);
      edate = NULL;
      
      relidtxt = sqlite3_mprintf(" %lld)", relid);
      if (!relidtxt)
      {
        sqlite3_free(p1extname);
        sqlite3_free(p2extname);
        free(daterange);
        setdberrtext("Out of Memory!");
        goto ErrGetDB_MainRelationshipList_Freethelist;
      }
      
      thelist[j] = (char *) malloc(sizeof(char)*(strlen(p1extname)+strlen(p2extname)+strlen(daterange)+strlen(relidtxt)+10));
      if (!thelist[j])
      {
        sqlite3_free(p1extname);
        sqlite3_free(p2extname);
        sqlite3_free(relidtxt);
        free(daterange);
        setdberrtext("Out of Memory!");
        goto ErrGetDB_MainRelationshipList_Freethelist;
      }
      strcpy(thelist[j], p1extname);
      strcat(thelist[j], " & ");
      strcat(thelist[j], p2extname);
      strcat(thelist[j], " (");
      strcat(thelist[j], daterange);
      strcat(thelist[j], ",");
      strcat(thelist[j], relidtxt);
      
      sqlite3_free(p1extname);
      p1extname = NULL;
      sqlite3_free(p2extname);
      p2extname = NULL;
      sqlite3_free(relidtxt);
      relidtxt = NULL;
      free(daterange);
      daterange = NULL;
      
      j++;
    }
  }
  
  rl++;
  rc = sqlite3_finalize(tblstmt);
  if (rc != SQLITE_OK) goto ErrGetDB_MainRelationshipList_Freethelist;
  
  return thelist;
  
  
ErrGetDB_MainRelationshipList_Freethelist:
  j += 0;
  sqlite3_int64 k;
  if (j) for (k = 0; k<j; k++) free(thelist[k]);
  free(thelist);
  
ErrGetDB_MainRelationshipList:
  if (errmsg)
  {
    setdberrtext(errmsg);
    sqlite3_free(errmsg);
  }
  lastdberr = rc;
  lastdberl = rl;
  return NULL;
}

ntsa getdb_mainsourcelist(int maxdesc)
{
  lastdberr = 0;
  lastdberl = 0;
  unsetdberrtext();
  if (!dbisopen) return NULL;
  
  char *errmsg = NULL;
  ntsa thelist;
  int rc, rl = 0;
  sqlite3_stmt *countstmt, *tblstmt;
  sqlite3_int64 tblcount;
  
  rl++;
  rc = sqlite3_prepare_v2(db, "SELECT Count(SourceID) FROM Source;", 36, &countstmt, NULL);
  if (rc != SQLITE_OK) goto ErrGetDB_MainSourceList;
  
  rl++;
  rc = sqlite3_step(countstmt);
  if (rc != SQLITE_ROW && rc != SQLITE_DONE && rc != SQLITE_OK) goto ErrGetDB_MainSourceList;
  
  rl++;
  tblcount = sqlite3_column_int64(countstmt,0);
  
  thelist = (ntsa) malloc(sizeof(char *) * (tblcount + 1));
  if (thelist == NULL)
  {
    setdberrtext("Out of Memory loading data from Database!");
    rc = 0;
    goto ErrGetDB_MainSourceList;
  }
  thelist[tblcount] = NULL;
  
  rl++;
  rc = sqlite3_finalize(countstmt);
  if (rc != SQLITE_OK)
  {
    goto ErrGetDB_MainSourceList;
  }
  
  if (tblcount == 0) return thelist;
  
  rl++;
  rc = sqlite3_prepare_v2(db, sql_select_mainsourcelist, -1, &tblstmt, NULL);
  if (rc != SQLITE_OK)
  {
    free(thelist);
    goto ErrGetDB_MainSourceList;
  }
  
  rl++;
  rc = SQLITE_ROW;
  sqlite3_int64 j=0, sourceid;
  const char *desc, *stype;
  char *ldesc, *typeandid;
  
  while (rc == SQLITE_ROW)
  {
    rc = sqlite3_step(tblstmt);
    if (rc != SQLITE_ROW && rc != SQLITE_DONE && rc != SQLITE_OK) goto ErrGetDB_MainSourceList_Freethelist;
    
    if (rc == SQLITE_ROW && j < tblcount)
    {
      desc = sqlite3_column_text(tblstmt, 0);
      stype = sqlite3_column_text(tblstmt, 1);
      sourceid = sqlite3_column_int64(tblstmt, 2);
      
      if (desc)
      {
        ldesc = (char *) malloc(sizeof(char)*(strlen(desc)+1));
        if (!ldesc)
        {
          setdberrtext("Out of Memory!");
          goto ErrGetDB_MainSourceList_Freethelist;
        }
        strcpy(ldesc, desc);
        if (maxdesc > 0 && strlen(ldesc) > maxdesc) ldesc[maxdesc] = 0;
      }
      else
      {
        ldesc = (char *) malloc(sizeof(char));
        if (!ldesc)
        {
          setdberrtext("Out of Memory!");
          goto ErrGetDB_MainSourceList_Freethelist;
        }
        ldesc[0] = 0;
      }
      
      typeandid = sqlite3_mprintf(" (%s, %lld)", stype, sourceid);
      if (!typeandid)
      {
        free(ldesc);
        setdberrtext("Out of Memory!");
        goto ErrGetDB_MainSourceList_Freethelist;
      }
      
      thelist[j] = (char *) malloc(sizeof(char)*(strlen(ldesc)+strlen(typeandid)+1));
      if (!thelist[j])
      {
        free(ldesc);
        sqlite3_free(typeandid);
        setdberrtext("Out of Memory!");
        goto ErrGetDB_MainSourceList_Freethelist;
      }
      strcpy(thelist[j],ldesc);
      strcat(thelist[j],typeandid);
      
      free(ldesc);
      ldesc = NULL;
      sqlite3_free(typeandid);
      typeandid = NULL;
      
      j++;
    }
  }
  
  rl++;
  rc = sqlite3_finalize(tblstmt);
  if (rc != SQLITE_OK) goto ErrGetDB_MainSourceList_Freethelist;
  
  return thelist;
  
  
ErrGetDB_MainSourceList_Freethelist:
  j += 0;
  sqlite3_int64 k;
  if (j) for (k = 0; k<j; k++) free(thelist[k]);
  free(thelist);
  
ErrGetDB_MainSourceList:
  if (errmsg)
  {
    setdberrtext(errmsg);
    sqlite3_free(errmsg);
  }
  lastdberr = rc;
  lastdberl = rl;
  return NULL;
}

sqlite3_int64 addnewperson(const char *bio, const char *ethnicity, 
                           const char *sex, const char *gender, 
                           const char *notes, int userelkidid, 
                           sqlite3_int64 relkidid, int isadopted,
                           sqlite3_int64 bdateid, const char *bplace, 
                           int usebaddr, sqlite3_int64 baddrid, 
                           int isdead, sqlite3_int64 ddateid, 
                           const char *dplace, int usedaddr,
                           sqlite3_int64 daddrid, sqlite3_int64 dageid, 
                           int usesource, sqlite3_int64 sourceid)
{
  lastdberr = 0;
  lastdberl = 0;
  unsetdberrtext();
  if (!dbisopen) return 0;
  
  /*char *errmsg = NULL;*/
  int rc, rl = 0;
  sqlite3_stmt *npersonstmt;
  sqlite3_int64 ans = 0;
  
  rl++;
  if (!bio || !sex || !bplace)
  {
    setdberrtext("Bio/Sex/Birth Place cannot be NULL!");
    goto ErrAddNewPerson;
  }
  if (isdead && !dplace)
  {
    setdberrtext("Death Place cannot be NULL if the person is dead!");
    goto ErrAddNewPerson;
  }
  if (toupper(sex[0]) != 'M' && toupper(sex[0]) != 'F' && sex[0] != '?')
  {
    setdberrtext("Invalid Sex!");
    goto ErrAddNewPerson;
  }
  if (isadopted > 1) isadopted = 1;
  
  rl++;
  rc = sqlite3_prepare_v2(db, sql_insert_newperson, -1, &npersonstmt, NULL);
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  rc = sqlite3_bind_text(npersonstmt, 1, bio, -1, SQLITE_TRANSIENT);
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  if (ethnicity)
  {
    rc = sqlite3_bind_text(npersonstmt, 2, ethnicity, -1, SQLITE_TRANSIENT);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 2);
  }
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  rc = sqlite3_bind_text(npersonstmt, 3, sex, 1, SQLITE_TRANSIENT);
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  if (gender)
  {
    rc = sqlite3_bind_text(npersonstmt, 4, gender, -1, SQLITE_TRANSIENT);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 4);
  }
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  if (notes)
  {
    rc = sqlite3_bind_text(npersonstmt, 5, notes, -1, SQLITE_TRANSIENT);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 5);
  }
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  if (userelkidid)
  {
    rc = sqlite3_bind_int64(npersonstmt, 6, relkidid);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 6);
  }
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  if (isadopted < 0)
  {
    rc = sqlite3_bind_null(npersonstmt, 7);
  }
  else
  {
    rc = sqlite3_bind_int(npersonstmt, 7, isadopted);
  }
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  rc = sqlite3_bind_int64(npersonstmt, 8, bdateid);
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  rc = sqlite3_bind_text(npersonstmt, 9, bplace, -1, SQLITE_TRANSIENT);
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  if (usebaddr)
  {
    rc = sqlite3_bind_int64(npersonstmt, 10, baddrid);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 10);
  }
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  if (isdead)
  {
    if (sqlite3_bind_int64(npersonstmt, 11, ddateid) != SQLITE_OK) goto ErrAddNewPerson;
    rl++;
    if (sqlite3_bind_text(npersonstmt, 12, dplace, -1, SQLITE_TRANSIENT) != SQLITE_OK) goto ErrAddNewPerson;
    rl++;
    if (usedaddr)
    {
      rc = sqlite3_bind_int64(npersonstmt, 13, daddrid);
    }
    else
    {
      rc = sqlite3_bind_null(npersonstmt, 13);
    }
    if (rc != SQLITE_OK) goto ErrAddNewPerson;
    rl++;
    if (sqlite3_bind_int64(npersonstmt, 14, dageid) != SQLITE_OK) goto ErrAddNewPerson; 
  }
  else
  {
    if (sqlite3_bind_null(npersonstmt, 11) != SQLITE_OK) goto ErrAddNewPerson;
    rl++;
    if (sqlite3_bind_null(npersonstmt, 12) != SQLITE_OK) goto ErrAddNewPerson;
    rl++;
    if (sqlite3_bind_null(npersonstmt, 13) != SQLITE_OK) goto ErrAddNewPerson;
    rl++;
    if (sqlite3_bind_null(npersonstmt, 14) != SQLITE_OK) goto ErrAddNewPerson;
  }
  
  rl++;
  if (usesource)
  {
    rc = sqlite3_bind_int64(npersonstmt, 15, sourceid);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 15);
  }
  if (rc != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  rc = sqlite3_step(npersonstmt);
  if (rc != SQLITE_ROW && rc != SQLITE_DONE && rc != SQLITE_OK)
  {
    setdberrtext(sqlite3_errmsg(db));
    goto ErrAddNewPerson;
  }
  
  rl++;
  if (sqlite3_finalize(npersonstmt) != SQLITE_OK) goto ErrAddNewPerson;
  
  rl++;
  ans = sqlite3_last_insert_rowid(db);
  
  /* --Return-- */
  return ans;
  
  
ErrAddNewPerson:
  /*if (errmsg)
  {
    setdberrtext(errmsg);
    sqlite3_free(errmsg);
  }*/
  lastdberr = rc;
  lastdberl = rl;
  return 0;
}

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
                    sqlite3_int64 sourceid)
{
  lastdberr = 0;
  lastdberl = 0;
  unsetdberrtext();
  if (!dbisopen) return 0;
  
  int rc, rl = 0;
  sqlite3_stmt *npersonstmt;
  sqlite3_int64 ans = 0;
  
  
  rl++;
  if (!personid)
  {
    setdberrtext("PersonID must be specified!");
    goto ErrModifyNewPerson;
  }
  if (!bio || !sex || !bplace)
  {
    setdberrtext("Bio/Sex/Birth Place cannot be NULL!");
    goto ErrModifyNewPerson;
  }
  if (isdead && !dplace)
  {
    setdberrtext("Death Place cannot be NULL if the person is dead!");
    goto ErrModifyNewPerson;
  }
  if (toupper(sex[0]) != 'M' && toupper(sex[0]) != 'F' && sex[0] != '?')
  {
    setdberrtext("Invalid Sex!");
    goto ErrModifyNewPerson;
  }
  if (isadopted > 1) isadopted = 1;
  
  rl++;
  rc = sqlite3_prepare_v2(db, sql_update_newperson, -1, &npersonstmt, NULL);
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  rc = sqlite3_bind_text(npersonstmt, 1, bio, -1, SQLITE_TRANSIENT);
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  if (ethnicity)
  {
    rc = sqlite3_bind_text(npersonstmt, 2, ethnicity, -1, SQLITE_TRANSIENT);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 2);
  }
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  rc = sqlite3_bind_text(npersonstmt, 3, sex, 1, SQLITE_TRANSIENT);
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  if (gender)
  {
    rc = sqlite3_bind_text(npersonstmt, 4, gender, -1, SQLITE_TRANSIENT);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 4);
  }
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  if (notes)
  {
    rc = sqlite3_bind_text(npersonstmt, 5, notes, -1, SQLITE_TRANSIENT);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 5);
  }
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  if (userelkidid)
  {
    rc = sqlite3_bind_int64(npersonstmt, 6, relkidid);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 6);
  }
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  if (isadopted < 0)
  {
    rc = sqlite3_bind_null(npersonstmt, 7);
  }
  else
  {
    rc = sqlite3_bind_int(npersonstmt, 7, isadopted);
  }
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  rc = sqlite3_bind_int64(npersonstmt, 8, bdateid);
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  rc = sqlite3_bind_text(npersonstmt, 9, bplace, -1, SQLITE_TRANSIENT);
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  if (usebaddr)
  {
    rc = sqlite3_bind_int64(npersonstmt, 10, baddrid);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 10);
  }
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  if (isdead)
  {
    if (sqlite3_bind_int64(npersonstmt, 11, ddateid) != SQLITE_OK) goto ErrModifyNewPerson;
    rl++;
    if (sqlite3_bind_text(npersonstmt, 12, dplace, -1, SQLITE_TRANSIENT) != SQLITE_OK) goto ErrModifyNewPerson;
    rl++;
    if (usedaddr)
    {
      rc = sqlite3_bind_int64(npersonstmt, 13, daddrid);
    }
    else
    {
      rc = sqlite3_bind_null(npersonstmt, 13);
    }
    if (rc != SQLITE_OK) goto ErrModifyNewPerson;
    rl++;
    if (sqlite3_bind_int64(npersonstmt, 14, dageid) != SQLITE_OK) goto ErrModifyNewPerson; 
  }
  else
  {
    if (sqlite3_bind_null(npersonstmt, 11) != SQLITE_OK) goto ErrModifyNewPerson;
    rl++;
    if (sqlite3_bind_null(npersonstmt, 12) != SQLITE_OK) goto ErrModifyNewPerson;
    rl++;
    if (sqlite3_bind_null(npersonstmt, 13) != SQLITE_OK) goto ErrModifyNewPerson;
    rl++;
    if (sqlite3_bind_null(npersonstmt, 14) != SQLITE_OK) goto ErrModifyNewPerson;
  }
  
  rl++;
  if (usesource)
  {
    rc = sqlite3_bind_int64(npersonstmt, 15, sourceid);
  }
  else
  {
    rc = sqlite3_bind_null(npersonstmt, 15);
  }
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  rc = sqlite3_bind_int64(npersonstmt, 16, personid);
  if (rc != SQLITE_OK) goto ErrModifyNewPerson;
  
  rl++;
  rc = sqlite3_step(npersonstmt);
  if (rc != SQLITE_ROW && rc != SQLITE_DONE && rc != SQLITE_OK)
  {
    setdberrtext(sqlite3_errmsg(db));
    goto ErrModifyNewPerson;
  }
  
  rl++;
  if (sqlite3_finalize(npersonstmt) != SQLITE_OK) goto ErrModifyNewPerson;
  
  /* --Return-- */
  return 1;
  
  
ErrModifyNewPerson:
  lastdberr = rc;
  lastdberl = rl;
  return 0;
}
