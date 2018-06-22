#ifndef __DBDATA_H__

#define __DBDATA_H__ 1

/* #define SQL_ST_BEGIN  0x040
#define SQL_ST_COMMIT 0x080
#define SQL_ST_CREATE 0x0C0
#define SQL_ST_INSERT 0x100
#define SQL_ST_DELETE 0x140
#define SQL_ST_UPDATE 0x180

#define SQL_TBL_PERSON          0x01
#define SQL_TBL_DATE            0x02
#define SQL_TBL_DATETYPE        0x03
#define SQL_TBL_CALENDARTYPE    0x04
#define SQL_TBL_AGE             0x05
#define SQL_TBL_AGETYPE         0x06
#define SQL_TBL_PERSONNAME      0x07
#define SQL_TBL_PHYSDESC        0x08
#define SQL_TBL_BELIEFS         0x09
#define SQL_TBL_BELIEFTYPE      0x0A
#define SQL_TBL_NATIONALITY     0x0B
#define SQL_TBL_AFFILTYPE       0x0C
#define SQL_TBL_ADOPTFOSTER     0x0D
#define SQL_TBL_ADOPTPERSONTYPE 0x0E
#define SQL_TBL_ADOPTTYPE       0x0F
#define SQL_TBL_RELATIONSHIP    0x10
#define SQL_TBL_RELTYPE         0x11
#define SQL_TBL_RELENDTYPE      0x12
#define SQL_TBL_JOB             0x13
#define SQL_TBL_ADDRESS         0x14
#define SQL_TBL_LIVINGADDR      0x15
#define SQL_TBL_WORKINGADDR     0x16
#define SQL_TBL_OTHERCONTACT    0x17
#define SQL_TBL_CONTACTTYPE     0x18
#define SQL_TBL_EDUCACHIEVE     0x19
#define SQL_TBL_EVENT           0x1A
#define SQL_TBL_SOURCE          0x1B
#define SQL_TBL_SOURCETYPE      0x1C
#define SQL_TBL_MULTIMEDIA      0x1D
#define SQL_TBL_INMULTIMEDIA    0x1E
#define SQL_TBL_ISO6393         0x1F
#define SQL_TBL_METAINFO        0x20 */

#ifndef __DBDATA_DECLARE__


extern char newdb_sql_begin[];
extern char newdb_sql_commit[];
extern char newdb_sql_vacuum[];

extern char newdb_sql_pragma_foreign_keys_on[];
extern char newdb_sql_pragma_foreign_keys_off[];

extern char newdb_sql_create_person[];
extern char newdb_sql_create_date[];
extern char newdb_sql_create_datetype[];
extern char newdb_sql_create_calendartype[];
extern char newdb_sql_create_age[];
extern char newdb_sql_create_agetype[];
extern char newdb_sql_create_personname[];
extern char newdb_sql_create_physdesc[];
extern char newdb_sql_create_beliefs[];
extern char newdb_sql_create_belieftype[];
extern char newdb_sql_create_nationality[];
extern char newdb_sql_create_affiltype[];
extern char newdb_sql_create_adoptfoster[];
extern char newdb_sql_create_adoptpersontype[];
extern char newdb_sql_create_adopttype[];
extern char newdb_sql_create_relationship[];
extern char newdb_sql_create_reltype[];
extern char newdb_sql_create_relendtype[];
extern char newdb_sql_create_job[];
extern char newdb_sql_create_address[];
extern char newdb_sql_create_livingaddr[];
extern char newdb_sql_create_workingaddr[];
extern char newdb_sql_create_othercontact[];
extern char newdb_sql_create_contacttype[];
extern char newdb_sql_create_educachieve[];
extern char newdb_sql_create_event[];
extern char newdb_sql_create_source[];
extern char newdb_sql_create_sourcetype[];
extern char newdb_sql_create_multimedia[];
extern char newdb_sql_create_inmultimedia[];
extern char newdb_sql_create_iso6393[];
extern char newdb_sql_create_metainfo[];

extern char newdb_sql_insert_datetype[];
extern char newdb_sql_insert_calendartype[];
extern char newdb_sql_insert_agetype[];
extern char newdb_sql_insert_belieftype[];
extern char newdb_sql_insert_affiltype[];
extern char newdb_sql_insert_adoptpersontype[];
extern char newdb_sql_insert_adopttype[];
extern char newdb_sql_insert_reltype[];
extern char newdb_sql_insert_relendtype[];
extern char newdb_sql_insert_contacttype[];
extern char newdb_sql_insert_sourcetype[];
extern char newdb_sql_insert_metainfo[];
extern char newdb_sql_insert_iso6393[];

extern char newdb_sql_update_metainfo_version[];
extern char newdb_sql_update_metainfo_last_edit[];

extern char sql_select_mainpersonlist[];
extern char sql_select_mainrelationshiplist[];
extern char sql_select_mainsourcelist[];

extern char sql_insert_newperson[];


#endif


#endif