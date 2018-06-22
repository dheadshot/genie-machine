#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iup/iup.h>
#include <iup/iup_config.h>
#include <errno.h>
#include <sqlite3.h>

#include "utf8.h"
#include "doerror.h"
#include "types.h"
#include "dbaccess.h"
#include "windowlist.h"

#include "textedit.h"

#include "newperson.h"



sqlite3_int64 newperson_personid = 0;



/* -------------- Callbacks -------------- */

int newperson_bioeditbtn_action_cb(Ihandle *bioeditbtn)
{
  Ihandle *dlg = IupGetDialog(bioeditbtn);
  Ihandle *bioprevlab = IupGetDialogChild(dlg, "BIO_PREVIEW_LAB");
  Ihandle *config = (Ihandle *) IupGetAttribute(dlg, "CONFIG");
  char *newbio = NULL;
  char *windowlist = IupGetAttribute(dlg, "WINDOW_LIST");
  int rc;
  
  char *oldbio = IupGetAttribute(dlg, "BIO_TEXT");
  if (oldbio)
  {
    rc = dotedit(config, oldbio, &newbio, "Edit Person's Biography", dlg, windowlist);
  }
  else
  {
    rc = dotedit(config, "", &newbio, "Edit Person's Biography", dlg, windowlist);
  }
  if (rc)
  {
    IupSetStrAttribute(dlg, "BIO_TEXT", newbio);
    char *tbio = (char *) malloc(sizeof(char)*(3+sstrlen(newbio))); //len+1+strlen("...") - 1 (since max if "..." is used will be < total len).
    if (!tbio)
    {
      show_error("Out of Memory!", 1, NULL, dlg);
    }
    else
    {
      strcpy_count(tbio, newbio, 22);
      if (strlen(newbio) > strlen(tbio)) strcat(tbio, "...");
      IupSetStrAttribute(bioprevlab, "TITLE", tbio);
      free(tbio);
    }
    free(newbio);
  }
  return IUP_DEFAULT;
}

int newperson_ethnicunktgl_action_cb(Ihandle *ethnicunktgl, int state)
{
  Ihandle *ethnictxt = IupGetDialogChild(ethnicunktgl, "ETHNIC_TXT");
  
  if (!state) IupSetAttribute(ethnictxt, "ACTIVE", "YES");
  else IupSetAttribute(ethnictxt, "ACTIVE", "NO");
  
  return IUP_DEFAULT;
}

int newperson_genunktgl_action_cb(Ihandle *genunktgl, int state)
{
  Ihandle *gentxt = IupGetDialogChild(genunktgl, "GENDER_TXT");
  
  if (!state) IupSetAttribute(gentxt, "ACTIVE", "YES");
  else IupSetAttribute(gentxt, "ACTIVE", "NO");
  
  return IUP_DEFAULT;
}

int newperson_nameseditbtn_action_cb(Ihandle *nameseditbtn)
{
  /* Launch nameslist dlg */
}

int newperson_noteeditbtn_action_cb(Ihandle *noteeditbtn)
{
  Ihandle *dlg = IupGetDialog(noteeditbtn);
  Ihandle *noteprevlab = IupGetDialogChild(dlg, "NOTES_PREVIEW_LAB");
  Ihandle *config = (Ihandle *) IupGetAttribute(dlg, "CONFIG");
  char *newnotes = NULL;
  char *windowlist = IupGetAttribute(dlg, "WINDOW_LIST");
  int rc;
  
  char *oldnotes = IupGetAttribute(dlg, "NOTES_TEXT");
  if (oldnotes)
  {
    rc = dotedit(config, oldnotes, &newnotes, "Edit Person's Notes", dlg, windowlist);
  }
  else
  {
    rc = dotedit(config, "", &newnotes, "Edit Person's Notes", dlg, windowlist);
  }
  if (rc)
  {
    IupSetStrAttribute(dlg, "NOTES_TEXT", newnotes);
    char *tnotes = (char *) malloc(sizeof(char)*(3+sstrlen(newnotes))); //len+1+strlen("...") - 1 (since max if "..." is used will be < total len).
    if (!tnotes)
    {
      show_error("Out of Memory!", 1, NULL, dlg);
    }
    else
    {
      strcpy_count(tnotes, newnotes, 22);
      if (strlen(newnotes) > strlen(tnotes)) strcat(tnotes, "...");
      IupSetStrAttribute(noteprevlab, "TITLE", tnotes);
      free(tnotes);
    }
    free(newnotes);
  }
  return IUP_DEFAULT;
}

int newperson_bplaceunktgl_action_cb(Ihandle *bplaceunktgl, int state)
{
  Ihandle *bplacetxt = IupGetDialogChild(bplaceunktgl, "BIRTH_PLACE_TXT");
  Ihandle *baddrlab = IupGetDialogChild(bplaceunktgl, "BIRTH_ADDRESS_LAB");
  Ihandle *baddreditbtn = IupGetDialogChild(bplaceunktgl, "BIRTH_ADDRESS_EDIT_BTN");
  Ihandle *baddrprevlab = IupGetDialogChild(bplaceunktgl, "BIRTH_ADDRESS_PREVIEW_LAB");
  Ihandle *baddrnatgl = IupGetDialogChild(bplaceunktgl, "BIRTH_ADDRESS_NA_TGL");
  
  if (!state)
  {
    IupSetAttribute(bplacetxt, "ACTIVE", "YES");
    IupSetAttribute(baddrlab, "ACTIVE", "YES");
    IupSetAttribute(baddrnatgl, "ACTIVE", "YES");
    if (!IupGetInt(baddrnatgl, "VALUE"))
    {
      IupSetAttribute(baddreditbtn, "ACTIVE", "YES");
      IupSetAttribute(baddrprevlab, "ACTIVE", "YES");
    }
  }
  else
  {
    IupSetAttribute(bplacetxt, "ACTIVE", "NO");
    IupSetAttribute(baddrlab, "ACTIVE", "NO");
    IupSetAttribute(baddreditbtn, "ACTIVE", "NO");
    IupSetAttribute(baddrprevlab, "ACTIVE", "NO");
    IupSetAttribute(baddrnatgl, "ACTIVE", "NO");
  }
  
  return IUP_DEFAULT;
}

int newperson_baddrnatgl_action_cb(Ihandle *baddrnatgl, int state)
{
  Ihandle *baddreditbtn = IupGetDialogChild(baddrnatgl, "BIRTH_ADDRESS_EDIT_BTN");
  Ihandle *baddrprevlab = IupGetDialogChild(baddrnatgl, "BIRTH_ADDRESS_PREVIEW_LAB");
  
  if (!state)
  {
    IupSetAttribute(baddreditbtn, "ACTIVE", "YES");
    char *htitle = IupGetAttribute(baddrprevlab, "OLD_TITLE");
    if (htitle) IupSetStrAttribute(baddrprevlab, "TITLE", htitle);
    else IupSetStrAttribute(baddrprevlab, "TITLE", "");
  }
  else
  {
    IupSetAttribute(baddreditbtn, "ACTIVE", "NO");
    char *curtitle = IupGetAttribute(baddrprevlab, "TITLE");
    if (curtitle) IupSetStrAttribute(baddrprevlab, "OLD_TITLE", curtitle);
    else IupSetStrAttribute(baddrprevlab, "OLD_TITLE", "");
    IupSetAttribute(baddrprevlab, "TITLE", "(Not Applicable!)");
  }
  
  return IUP_DEFAULT;
}

int newperson_dalivetgl_action_cb(Ihandle *dalivetgl, int state)
{
  Ihandle *dfrm = IupGetDialogChild(dalivetgl, "DEATH_FRM");
  
  if (!state)
  {
    IupSetAttribute(dfrm, "ACTIVE", "YES");
  }
  else
  {
    IupSetAttribute(dfrm, "ACTIVE", "NO");
  }
  
  return IUP_DEFAULT;
}

int newperson_dplaceunktgl_action_cb(Ihandle *dplaceunktgl, int state)
{
  Ihandle *dplacetxt = IupGetDialogChild(dplaceunktgl, "DEATH_PLACE_TXT");
  Ihandle *daddrlab = IupGetDialogChild(dplaceunktgl, "DEATH_ADDRESS_LAB");
  Ihandle *daddreditbtn = IupGetDialogChild(dplaceunktgl, "DEATH_ADDRESS_EDIT_BTN");
  Ihandle *daddrprevlab = IupGetDialogChild(dplaceunktgl, "DEATH_ADDRESS_PREVIEW_LAB");
  Ihandle *daddrnatgl = IupGetDialogChild(dplaceunktgl, "DEATH_ADDRESS_NA_TGL");
  
  if (!state)
  {
    IupSetAttribute(dplacetxt, "ACTIVE", "YES");
    IupSetAttribute(daddrlab, "ACTIVE", "YES");
    IupSetAttribute(daddrnatgl, "ACTIVE", "YES");
    if (!IupGetInt(daddrnatgl, "VALUE"))
    {
      IupSetAttribute(daddreditbtn, "ACTIVE", "YES");
      IupSetAttribute(daddrprevlab, "ACTIVE", "YES");
    }
  }
  else
  {
    IupSetAttribute(dplacetxt, "ACTIVE", "NO");
    IupSetAttribute(daddrlab, "ACTIVE", "NO");
    IupSetAttribute(daddreditbtn, "ACTIVE", "NO");
    IupSetAttribute(daddrprevlab, "ACTIVE", "NO");
    IupSetAttribute(daddrnatgl, "ACTIVE", "NO");
  }
  
  return IUP_DEFAULT;
}

int newperson_daddrnatgl_action_cb(Ihandle *daddrnatgl, int state)
{
  Ihandle *daddreditbtn = IupGetDialogChild(daddrnatgl, "DEATH_ADDRESS_EDIT_BTN");
  Ihandle *daddrprevlab = IupGetDialogChild(daddrnatgl, "DEATH_ADDRESS_PREVIEW_LAB");
  
  if (!state)
  {
    IupSetAttribute(daddreditbtn, "ACTIVE", "YES");
    char *htitle = IupGetAttribute(daddrprevlab, "OLD_TITLE");
    if (htitle) IupSetStrAttribute(daddrprevlab, "TITLE", htitle);
    else IupSetStrAttribute(daddrprevlab, "TITLE", "");
  }
  else
  {
    IupSetAttribute(daddreditbtn, "ACTIVE", "NO");
    char *curtitle = IupGetAttribute(daddrprevlab, "TITLE");
    if (curtitle) IupSetStrAttribute(daddrprevlab, "OLD_TITLE", curtitle);
    else IupSetStrAttribute(daddrprevlab, "OLD_TITLE", "");
    IupSetAttribute(daddrprevlab, "TITLE", "(Not Applicable!)");
  }
  
  return IUP_DEFAULT;
}

int newperson_cancelbtn_action_cb(Ihandle *ih)
{
  Ihandle *dlg = IupGetDialog(ih);
  
  if (IupAlarm("Warning", 
      "Are you sure you want to close the New Person dialogue box?  The person will not be added to the database and all the details you have included will be lost!", "Yes", "No", NULL) != 1)
  {
    return IUP_IGNORE;
  }
  
  IupSetAttribute(dlg, "STATUS", "0");
  IupSetAttribute(dlg, "CONFIG", NULL);
  return IUP_CLOSE;
}

int newperson_helpbtn_action_cb(Ihandle *ih)
{
  if (IupHelp("https://github.com/dheadshot/genie-machine") < 1) //TODO: Change this to the correct HELP link!
  {
    show_error("Error opening help file!  Please set the web browser for your system in the main window, using \"Help>Set Browser...\".",1,"Error Opening Help",IupGetDialog(ih));
  }
  return IUP_DEFAULT;
}




/* -------------- Main -------------- */

sqlite3_int64 addorupdaterecord(Ihandle *dlg, sqlite3_int64 recordid)
{
  Ihandle *ethnictxt = IupGetDialogChild(dlg, "ETHNIC_TXT");
  Ihandle *ethnicunktgl = IupGetDialogChild(dlg, "ETHNIC_UNKNOWN_TGL");
  Ihandle *csexrbox = IupGetDialogChild(dlg, "CSEX_RBOX");
  Ihandle *csexmtgl = IupGetDialogChild(dlg, "CSEX_MALE_TGL");
  Ihandle *csexftgl = IupGetDialogChild(dlg, "CSEX_FEMALE_TGL");
  Ihandle *csexutgl = IupGetDialogChild(dlg, "CSEX_UNKNOWN_TGL");
  Ihandle *genunktgl = IupGetDialogChild(dlg, "GENDER_UNKNOWN_TGL");
  Ihandle *gentxt = IupGetDialogChild(dlg, "GENDER_TXT");
  Ihandle *bplacetxt = IupGetDialogChild(dlg, "BIRTH_PLACE_TXT");
  Ihandle *bplaceunktgl = IupGetDialogChild(dlg, "BIRTH_PLACE_UNKNOWN_TGL");
  Ihandle *baddrnatgl = IupGetDialogChild(dlg, "BIRTH_ADDRESS_NA_TGL");
  Ihandle *dalivetgl = IupGetDialogChild(dlg, "DEATH_ALIVE_TGL");
  Ihandle *dplacetxt = IupGetDialogChild(dlg, "DEATH_PLACE_TXT");
  Ihandle *dplaceunktgl = IupGetDialogChild(dlg, "DEATH_PLACE_UNKNOWN_TGL");
  Ihandle *daddrnatgl = IupGetDialogChild(dlg, "DEATH_ADDRESS_NA_TGL");
  
  sqlite3_int64 ans = 0;
  
  IupSetStrAttribute(dlg, "UNKNOWN_DEFAULT", "?");
  IupSetStrAttribute(dlg, "BLANK_DEFAULT", "");
  char *unkdef = IupGetAttribute(dlg, "UNKNOWN_DEFAULT");
  char *blkdef = IupGetAttribute(dlg, "BLANK_DEFAULT");
  
  /* Set Fields */
  char *bio = IupGetAttribute(dlg, "BIO_TEXT");
  if (!bio) bio = blkdef;
  
  char *ethnicity;
  if (IupGetInt(ethnicunktgl, "VALUE")) ethnicity = unkdef;
  else ethnicity = IupGetAttribute(ethnictxt, "VALUE");
  
  Ihandle *csexactive = IupGetAttribute(csexrbox, "VALUE_HANDLE");
  if (csexactive == csexmtgl) IupSetStrAttribute(csexrbox, "VAL_CHAR", "M");
  else if (csexactive == csexftgl) IupSetStrAttribute(csexrbox, "VAL_CHAR", "F");
  else if (csexactive == csexutgl) IupSetStrAttribute(csexrbox, "VAL_CHAR", "?");
  else
  {
    show_error("Chromosonal Sex not selected!  You must specify a chromosonal sex.", 1, NULL, dlg);
    IupSetAttribute(dlg, "UNKNOWN_DEFAULT", NULL);
    IupSetAttribute(dlg, "BLANK_DEFAULT", NULL);
    return 0;
  }
  
  char *gender;
  if (IupGetInt(genunktgl, "VALUE")) gender = unkdef;
  else gender = IupGetAttribute(gentxt, "VALUE");
  
  char *notes = IupGetAttribute(dlg, "NOTES_TEXT");
  
  sqlite3_int64 *brelref = (sqlite3_int64 *) IupGetAttribute(dlg, "BREL_VALUE");
  int usebrel = 1;
  sqlite3_int64 brel = 0;
  if (!brelref || !(*brelref)) usebrel = 0;
  else sqlite3_int64 = (*brelref);
  
  int isadopted = -1;
  
  sqlite3_int64 *bdateref = (sqlite3_int64 *) IupGetAttribute(dlg, "BDATE_VALUE");
  sqlite3_int64 bdate = 0;
  if (bdateref && (*bdateref)) bdate = (*bdateref);
  
  char *bplace;
  if (IupGetInt(bplaceunktgl, "VALUE")) bplace = unkdef;
  else bplace = IupGetAttribute(bplacetxt, "VALUE");
  
  int usebaddr = 0;
  sqlite3_int64 baddr = 0, *baddrref = NULL;
  if (!IupGetInt(bplaceunktgl, "VALUE") && !IupGetInt(baddrnatgl, "VALUE"))
  {
    baddrref = (sqlite3_int64 *) IupGetAttribute(dlg, "BADDR_VALUE");
    if (baddrref && (*baddrref))
    {
      usebaddr = 1;
      baddr = (*baddrref);
    }
  }
  
  int isdead = 1;
  sqlite3_int64 ddate = 0, *ddateref = NULL;
  char *dplace = NULL;
  int usedaddr = 0;
  sqlite3_int64 daddr = 0, *daddrref = NULL;
  sqlite3_int64 dage = 0, *dageref = NULL;
  if (IupGetInt(dalivetgl, "VALUE")) isdead = 0;
  else
  {
    ddateref = (sqlite3_int64 *) IupGetAttribute(dlg, "DDATE_VALUE");
    if (ddateref && (*ddateref)) ddate = (*ddateref);
    
    if (IupGetInt(dplaceunktgl, "VALUE")) dplace = unkdef;
    else IupGetAttribute(dplacetxt, "VALUE");
    
    if (!IupGetInt(daddrnatgl, "VALUE")) && !IupGetInt(dplaceunktgl, "VALUE"))
    {
      daddrref = (sqlite3_int64 *) IupGetAttribute(dlg, "DADDR_VALUE");
      if (daddrref && (*daddrref))
      {
        daddr = (*daddrref);
        usedaddr = 1;
      }
    }
    
    dageref = (sqlite3_int64 *) IupGetAttribute(dlg, "DAGE_VALUE");
    if (dageref && (*dageref)) dage = (*dageref);
  }
  
  sqlite3_int64 source = 0, *sourceref = NULL;
  int usesource = 0;
  
  sourceref = (sqlite3_int64 *) IupGetAttribute(dlg, "SOURCE_VALUE");
  if (sourceref && (*sourceref))
  {
    usesource = 1;
    source = (*sourceref);
  }
  
  /* Send to database */
  if (recordid)
  {
    /* Update */
    /* TODO: Code to Update the database! */
  }
  else
  {
    /* Insert */
    ans = addnewperson(bio, ethnicity, csex, gender, notes, usebrel, 
                       brel, isadopted, bdate, bplace, usebaddr, 
                       baddr, isdead, ddate, dplace, usedaddr, daddr, 
                       dage, usesource, source);
    if (!ans)
    {
      /* Handle error */
      char *errmsg = NULL;
      if (lastdberrtext)
      {
        errmsg = (char *) malloc(sizeof(char)*(101+strlen(lastdberrtext)));
        if (!errmsg) show_error("Error committing data to database! Additionally, error processing the error!", 1, NULL, dlg);
        else
        {
          sprintf(errmsg, "There was the following error sending data to the database: (%lu:%lu) %s", getlastdberr(), lastdberl, lastdberrtext);
          show_error(errmsg, 1, "Database Error", dlg);
          	fprintf(stderr, errmsg);
          free(errmsg);
        }
      }
      else
      {
        errmsg = (char *) malloc(sizeof(char)*101);
        if (!errmsg) show_error("Error committing data to database! Additionally, error processing the error!", 1, NULL, dlg);
        else
        {
        sprintf(errmsg, "There was an error %lu:%lu sending data to the database!", getlastdberr(), lastdberl);
          show_error(errmsg, 1, "Database Error", dlg);
          	fprintf(stderr, errmsg);
          free(errmsg);
        }
      }
    }
    
  }
  
  IupSetAttribute(dlg, "UNKNOWN_DEFAULT", NULL);
  IupSetAttribute(dlg, "BLANK_DEFAULT", NULL);
  
  return ans;
}

sqlite3_int64 donewperson(Ihandle *parentdlg, Ihandle *config, const char *prevwindows)
{
  Ihandle *npdlg, *vbox;
  Ihandle *biolab, *bioprevlab, *bioeditbtn, *biohbox;
  Ihandle *ethniclab, *ethnictxt, *ethnicunktgl, *ethnichbox;
  Ihandle *csexlab, *csexrbox, *csexhbox, *csexmopt, *csexfopt, *csexunkopt;
  Ihandle *genlab, *gentxt, *genunktgl, *genhbox;
  Ihandle *nameslab, *nameseditbtn, *namesprevlab, *nameshbox;
  Ihandle *notelab, *noteeditbtn, *noteprevlab, *notehbox;
  Ihandle *bfrm, *bvbox;
  Ihandle *brellab, *brelchoosebtn, *brelprevlab, *brelhbox;
  /* Ihandle *adoptlab, *adopttgl; //Use AdoptFoster table instead. */
  Ihandle *bdatelab, *bdateeditbtn, *bdateprevlab, *bdatehbox;
  Ihandle *bplacelab, *bplacetxt, *bplaceunktgl, *bplacehbox;
  Ihandle *baddrlab, *baddrprevlab, *baddreditbtn, *baddrnatgl, *baddrhbox;
  Ihandle *dfrm, *dvbox, *dalivetgl;
  Ihandle *ddatelab, *ddateprevlab, *ddateeditbtn, *ddatehbox;
  Ihandle *dplacelab, *dplacetxt, *dplaceunktgl, *dplacehbox;
  Ihandle *daddrlab, *daddrprevlab, *daddreditbtn, *daddrnatgl, *daddrhbox;
  Ihandle *dagelab, *dageprevlab, *dageeditbtn, *dagehbox;
  Ihandle *srclab, *srcprevlab, *srcchoosebtn, *srchbox;
  Ihandle *okbtn, *cancelbtn, *helpbtn, *hbox1;
  
  int dlgans = 0;
  
  newperson_personid = 0;
  
  /* Generate the current WindowList */
  
  char *windowlist = NULL;
  
  if (!duplicatewindowlist(&windowlist, prevwindows) || !addwindowtolist(&windowlist, "newperson"))
  {
    if (windowlist) freewindowlist(&windowlist);
    show_error("Out of Memory!", 1, NULL, parentdlg);
    return 0;
  }
  
  
  /* Generate the Window */
  
  biolab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(biolab, "TITLE", "Person's Biography:");
  
  /*biotxt = IupText(NULL);
  IupSetAttribute(biotxt, "NAME", "BIO_TXT");*/
  
  bioprevlab = IupLabel(NULL);
  IupSetAttribute(bioprevlab, "TITLE", "(Empty!)");
  IupSetAttribute(bioprevlab, "NAME", "BIO_PREVIEW_LAB");
  IupSetAttribute(bioprevlab, "EXPAND", "HORIZONTAL");
  
  bioeditbtn = IupButton("Edit Biography...", NULL);
  IupSetAttribute(bioeditbtn, "NAME", "BIO_EDIT_BTN");
  IupSetCallback(bioeditbtn, "ACTION", (Icallback) newperson_bioeditbtn_action_cb); //Attribute is "BIO_TEXT"
  
  biohbox = IupHbox(bioeditbtn, bioprevlab, NULL); //<-Enclose
  
  ethniclab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(ethniclab, "TITLE", "Description of Person's Ethnicity:");
  
  ethnictxt = IupText(NULL);
  IupSetAttribute(ethnictxt, "NAME", "ETHNIC_TXT");
  IupSetAttribute(ethnictxt, "EXPAND", "HORIZONTAL");
  
  ethnicunktgl = IupToggle("Unknown Ethnicity", NULL);
  IupSetAttribute(ethnicunktgl, "NAME", "ETHNIC_UNKNOWN_TGL");
  IupSetAttribute(ethnicunktgl, "VALUE", "OFF");
  IupSetCallback(ethnicunktgl, "ACTION", (Icallback) newperson_ethnicunktgl_action_cb);
  
  ethnichbox = IupHbox(ethnictxt, ethnicunktgl, NULL); //<-Enclose
  
  csexlab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(csexlab, "TITLE", "(Chromosonal) Sex:");
  
  csexmopt = IupToggle("Has at least one Y Chromosone\n(Usually Male)", NULL);
  IupSetAttribute(csexmopt, "NAME", "CSEX_MALE_TGL");
  IupSetAttribute(csexmopt, "VALUE", "OFF");
  
  csexfopt = IupToggle("Has No Y Chromosones\n(Usually Female)", NULL);
  IupSetAttribute(csexfopt, "NAME", "CSEX_FEMALE_TGL");
  IupSetAttribute(csexfopt, "VALUE", "OFF");
  
  csexunkopt = IupToggle("Unknown", NULL);
  IupSetAttribute(csexunkopt, "NAME", "CSEX_UNKNOWN_TGL");
  IupSetAttribute(csexunkopt, "VALUE", "ON");
  
  csexhbox = IupHbox(csexmopt, csexfopt, csexunkopt, NULL);
  csexrbox = IupRadio(csexhbox); //<-Enclose
  IupSetAttributeHandle(csexrbox, "VALUE_HANDLE", csexunkopt);
  IupSetAttribute(csexrbox, "NAME", "CSEX_RBOX");
  
  genlab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(genlab, "TITLE", "Gender");
  
  gentxt = IupText(NULL);
  IupSetAttribute(gentxt, "NAME", "GENDER_TXT");
  IupSetAttribute(gentxt, "EXPAND", "HORIZONTAL");
  
  genunktgl = IupToggle("Unknown Gender", NULL);
  IupSetAttribute(genunktgl, "NAME", "GENDER_UNKNOWN_TGL");
  IupSetAttribute(genunktgl, "VALUE", "OFF");
  IupSetCallback(genunktgl, "ACTION", (Icallback) newperson_genunktgl_action_cb);
  
  genhbox = IupHbox(gentxt, genunktgl, NULL); //<-Enclose
  
  nameslab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(nameslab, "TITLE", "Names:");
  
  namesprevlab = IupLabel(NULL);
  IupSetAttribute(namesprevlab, "TITLE", "(None!)");
  IupSetAttribute(namesprevlab, "NAME", "NAMES_PREVIEW_LAB");
  IupSetAttribute(namesprevlab, "EXPAND", "HORIZONTAL");
  
  nameseditbtn = IupButton("Edit Names...", NULL);
  IupSetAttribute(nameseditbtn, "NAME", "NAMES_EDIT_BTN"); //TODO: CB this!
  
  nameshbox = IupHbox(nameseditbtn, namesprevlab, NULL); //<-Enclose
  
  notelab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(notelab, "TITLE", "Notes:");
  
  noteprevlab = IupLabel(NULL);
  IupSetAttribute(noteprevlab, "TITLE", "(Empty!)");
  IupSetAttribute(noteprevlab, "NAME", "NOTES_PREVIEW_LAB");
  IupSetAttribute(noteprevlab, "EXPAND", "HORIZONTAL");
  
  noteeditbtn = IupButton("Edit Notes...", NULL);
  IupSetAttribute(noteeditbtn, "NAME", "NOTES_EDIT_BTN");
  IupSetCallback(noteeditbtn, "ACTION", (Icallback) newperson_noteeditbtn_action_cb); //Attribute is "NOTES_TEXT"
  
  notehbox = IupHbox(noteeditbtn, noteprevlab, NULL); //<-Enclose
  
  brellab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(brellab, "TITLE", "Born of Relationship:");
  
  brelchoosebtn = IupButton("Choose Relationship...", NULL);
  IupSetAttribute(brelchoosebtn, "NAME", "BIRTH_RELATIONSHIP_CHOOSE_BTN"); //TODO: CB This!
  
  brelprevlab = IupLabel(NULL);
  IupSetAttribute(brelprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(brelprevlab, "NAME", "BIRTH_RELATIONSHIP_PREVIEW_LAB");
  IupSetAttribute(brelprevlab, "EXPAND", "HORIZONTAL");
  
  brelhbox = IupHbox(brelchoosebtn, brelprevlab, NULL); //<--Enclose
  
  bdatelab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(bdatelab, "TITLE", "Date of Birth:");
  
  bdateprevlab = IupLabel(NULL);
  IupSetAttribute(bdateprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(bdateprevlab, "NAME", "BIRTH_DATE_PREVIEW_LAB");
  IupSetAttribute(bdateprevlab, "EXPAND", "HORIZONTAL");
  
  bdateeditbtn = IupButton("Edit Date...", NULL);
  IupSetAttribute(bdateeditbtn, "NAME", "BIRTH_DATE_EDIT_BTN"); //TODO: CB This!
  
  bdatehbox = IupHbox(bdateeditbtn, bdateprevlab, NULL); //<--Enclose
  
  bplacelab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(bplacelab, "TITLE", "Place of Birth");
  
  bplacetxt = IupText(NULL);
  IupSetAttribute(bplacetxt, "NAME", "BIRTH_PLACE_TXT");
  IupSetAttribute(bplacetxt, "EXPAND", "HORIZONTAL");
  
  bplaceunktgl = IupToggle("Unknown Place of Birth", NULL);
  IupSetAttribute(bplaceunktgl, "NAME", "BIRTH_PLACE_UNKNOWN_TGL");
  IupSetAttribute(bplaceunktgl, "VALUE", "OFF");
  IupSetCallback(bplaceunktgl, "ACTION", (Icallback) newperson_bplaceunktgl_action_cb);
  
  bplacehbox = IupHbox(bplacetxt, bplaceunktgl, NULL); //<--Enclose
  
  baddrlab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(baddrlab, "TITLE", "Address of Birthplace:");
  IupSetAttribute(baddrlab, "NAME", "BIRTH_ADDRESS_LAB");
  
  baddrprevlab = IupLabel(NULL);
  IupSetAttribute(baddrprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(baddrprevlab, "NAME", "BIRTH_ADDRESS_PREVIEW_LAB");
  IupSetAttribute(baddrprevlab, "EXPAND", "HORIZONTAL");
  
  baddreditbtn = IupButton("Edit Address...", NULL);
  IupSetAttribute(baddreditbtn, "NAME", "BIRTH_ADDRESS_EDIT_BTN"); //TODO: CB this!
  
  baddrnatgl = IupToggle("Birthplace Has No Address", NULL);
  IupSetAttribute(baddrnatgl, "NAME", "BIRTH_ADDRESS_NA_TGL");
  IupSetAttribute(baddrnatgl, "VALUE", "OFF");
  IupSetCallback(baddrnatgl, "ACTION", (Icallback) newperson_baddrnatgl_action_cb);
  
  baddrhbox = IupHbox(baddreditbtn, baddrprevlab, baddrnatgl, NULL); //<--Enclose
  
  bvbox = IupVbox(brellab, brelhbox, bdatelab, bdatehbox, bplacelab, bplacehbox, baddrlab, baddrhbox, NULL);
  IupSetAttribute(bvbox, "MARGIN", "10x5");
  IupSetAttribute(bvbox, "GAP", "5");
  bfrm = IupFrame(bvbox); //<-Enclose
  IupSetAttribute(bfrm, "TITLE", "Birth Details:");
  
  dalivetgl = IupToggle("Person is &Alive", NULL); //<-Enclose
  IupSetAttribute(dalivetgl, "NAME", "DEATH_ALIVE_TGL");
  IupSetAttribute(dalivetgl, "VALUE", "OFF");
  IupSetCallback(dalivetgl, "ACTION", (Icallback) newperson_dalivetgl_action_cb);
  
  ddatelab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(ddatelab, "TITLE", "Date of Death:");
  
  ddateprevlab = IupLabel(NULL);
  IupSetAttribute(ddateprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(ddateprevlab, "NAME", "DEATH_DATE_PREVIEW_LAB");
  IupSetAttribute(ddateprevlab, "EXPAND", "HORIZONTAL");
  
  ddateeditbtn = IupButton("Edit Date...", NULL);
  IupSetAttribute(ddateeditbtn, "NAME", "DEATH_DATE_EDIT_BTN"); //TODO: CB this!
  
  ddatehbox = IupHbox(ddateeditbtn, ddateprevlab, NULL); //<--Enclose
  
  dplacelab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(dplacelab, "TITLE", "Place of Death");
  
  dplacetxt = IupText(NULL);
  IupSetAttribute(dplacetxt, "NAME", "DEATH_PLACE_TXT");
  IupSetAttribute(dplacetxt, "EXPAND", "HORIZONTAL");
  
  dplaceunktgl = IupToggle("Unknown Place of Death", NULL);
  IupSetAttribute(dplaceunktgl, "NAME", "DEATH_PLACE_UNKNOWN_TGL");
  IupSetAttribute(dplaceunktgl, "VALUE", "OFF");
  IupSetCallback(dplaceunktgl, "ACTION", (Icallback) newperson_dplaceunktgl_action_cb);
  
  dplacehbox = IupHbox(dplacetxt, dplaceunktgl, NULL); //<--Enclose
  
  daddrlab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(daddrlab, "TITLE", "Address of Deathplace:");
  IupSetAttribute(daddrlab, "NAME", "DEATH_ADDRESS_LAB");
  
  daddrprevlab = IupLabel(NULL);
  IupSetAttribute(daddrprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(daddrprevlab, "NAME", "DEATH_ADDRESS_PREVIEW_LAB");
  IupSetAttribute(daddrprevlab, "EXPAND", "HORIZONTAL");
  
  daddreditbtn = IupButton("Edit Address...", NULL);
  IupSetAttribute(daddreditbtn, "NAME", "DEATH_ADDRESS_EDIT_BTN"); //TODO: CB This!
  
  daddrnatgl = IupToggle("Deathplace Has No Address", NULL);
  IupSetAttribute(daddrnatgl, "NAME", "DEATH_ADDRESS_NA_TGL");
  IupSetAttribute(daddrnatgl, "VALUE", "OFF");
  
  daddrhbox = IupHbox(daddreditbtn, daddrprevlab, daddrnatgl, NULL); //<--Enclose
  
  dagelab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(dagelab, "TITLE", "Age at Death:");
  
  dageprevlab = IupLabel(NULL);
  IupSetAttribute(dageprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(dageprevlab, "NAME", "DEATH_AGE_PREVIEW_LAB");
  IupSetAttribute(dageprevlab, "EXPAND", "HORIZONTAL");
  
  dageeditbtn = IupButton("Edit Age...", NULL);
  IupSetAttribute(dageeditbtn, "NAME", "DEATH_AGE_EDIT_BTN"); //TODO: CB This!
  
  dagehbox = IupHbox(dageeditbtn, dageprevlab, NULL); //<--Enclose
  
  dvbox = IupVbox(ddatelab, ddatehbox, dplacelab, dplacehbox, daddrlab, daddrhbox, dagelab, dagehbox, NULL);
  IupSetAttribute(dvbox, "MARGIN", "10x5");
  IupSetAttribute(dvbox, "GAP", "5");
  dfrm = IupFrame(dvbox); //<-Enclose
  IupSetAttribute(dfrm, "TITLE", "Death Details:");
  IupSetAttribute(dfrm, "NAME", "DEATH_FRM");
  
  srclab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(srclab, "TITLE", "Source:");
  
  srcchoosebtn = IupButton("Choose Source...", NULL);
  IupSetAttribute(srcchoosebtn, "NAME", "SOURCE_CHOOSE_BTN"); //TODO: CB This!
  
  srcprevlab = IupLabel(NULL);
  IupSetAttribute(srcprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(srcprevlab, "NAME", "SOURCE_PREVIEW_LAB");
  IupSetAttribute(srcprevlab, "EXPAND", "HORIZONTAL");
  
  srchbox = IupHbox(srcchoosebtn, srcprevlab, NULL); //<-Enclose
  
  okbtn = IupButton("&OK", NULL);
  IupSetAttribute(okbtn, "NAME", "OK_BTN");
  IupSetAttribute(okbtn, "ACTIVE", "NO");
  IupSetAttribute(okbtn, "PADDING", "10x2"); //TODO: CB This!
  
  cancelbtn = IupButton("&Cancel", NULL);
  IupSetAttribute(cancelbtn, "NAME", "CANCEL_BTN");
  IupSetAttribute(cancelbtn, "PADDING", "10x2"); //TODO: CB This!
  
  helpbtn = IupButton("&Help...", NULL);
  IupSetAttribute(helpbtn, "NAME", "HELP_BTN");
  IupSetAttribute(helpbtn, "PADDING", "10x2");
  IupSetCallback(helpbtn, "ACTION", (Icallback) newperson_helpbtn_action_cb);
  
  hbox1 = IupHbox(IupFill(), okbtn, cancelbtn, helpbtn, NULL); //<-Enclose
  
  vbox = IupVbox(biolab, biohbox, ethniclab, ethnichbox, csexlab, csexrbox, genlab, genhbox, nameslab, nameshbox, notelab, notehbox, bfrm, dalivetgl, dfrm, srclab, srchbox, hbox1, NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "5");
  
  npdlg = IupDialog(vbox);
  IupSetAttribute(npdlg, "STATUS", "0");
  IupSetAttribute(npdlg, "TITLE", "Create New Person");
  IupSetAttribute(npdlg, "DIALOGFRAME", "YES");
  IupSetAttribute(npdlg, "CONFIG", (char *) config);
  IupSetAttributeHandle(npdlg, "DEFAULTENTER", okbtn);
  IupSetAttributeHandle(npdlg, "DEFAULTESC", cancelbtn);
  IupSetAttributeHandle(npdlg, "PARENTDIALOG", parentdlg);
  
  IupSetCallback(npdlg, "CLOSE_CB", (Icallback) newperson_cancelbtn_action_cb);
  
  IupSetAttribute(npdlg, "WINDOW_LIST", windowlist); //Remember to set to NULL at end, then free!
  
  IupPopup(npdlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
  
  if (IupGetInt(npdlg, "STATUS"))
  {
    //OK
    if (newperson_personid)
    {
      //Update
    }
    else
    {
      //Create
    }
  }
  
  IupSetAttribute(npdlg, "CONFIG", NULL);
  IupSetAttribute(npdlg, "WINDOW_LIST", NULL);
  freewindowlist(&windowlist);
  
  /* TODO: Finish this! */
}

