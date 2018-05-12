#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

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
                 const char *titleprefix, char *titlesuffix, 
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
  if (rc == SQLITE_OK || rc == SQLITE_DONE) return 1;
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

ntsa getdb_mainpersonlist()
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
  
  if (tblcount) return thelist;
  
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
  char *familyname, *givenname, *patronymic, *othernames, *titleprefix, 
       *titlesuffix, *titleinterpart, *nameformat, *datetype, *year1, *year2, 
       *fullname = NULL, *byear = NULL, *personidtxt = NULL;
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
      
      fullname = formatname(familyname, givenname, patronymic, othernames, 
                            titleprefix, titlesuffix, titleinterpart, 
                            nameformat);
      if (fullname == NULL)
      {
        goto ErrGetDB_MainPersonList_Freethelist;
      }
      
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
