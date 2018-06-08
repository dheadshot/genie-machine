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

#include "newperson.h"

sqlite3_int64 donewperson(Ihandle *parentdlg, Ihandle *config, const char *prevwindows)
{
  Ihandle *npdlg, *vbox;
  Ihandle *biolab, *bioprevlab, *bioeditbtn, *biohbox;
  Ihandle *ethniclab, *ethnictxt, *ethnicunktgl, *ethnichbox;
  Ihandle *csexlab, *csexrbox, *csexhbox, *csexmopt, *csexfopt, *csexunkopt;
  Ihandle *genlab, *gentxt, *genunktgl, *genhbox;
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
  Ihandle *srclab, *srcprevlab, *srcchoosebtn;
  Ihandle *okbtn, *cancelbtn, *helpbtn, *hbox1;
  
  int dlgans = 0;
  sqlite3_int64 personid = 0;
  
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
  
  biohbox = IupHbox(bioeditbtn, bioprevlab, NULL); //<-Enclose
  
  ethniclab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(ethniclab, "TITLE", "Description of Person's Ethnicity:");
  
  ethnictxt = IupText(NULL);
  IupSetAttribute(ethnictxt, "NAME", "ETHNIC_TXT");
  IupSetAttribute(ethnictxt, "EXPAND", "HORIZONTAL");
  
  ethnicunktgl = IupToggle("Unknown Ethnicity", NULL);
  IupSetAttribute(ethnicunktgl, "NAME, "ETHNIC_UNKNOWN_TGL");
  IupSetAttribute(ethnicunktgl, "VALUE, "OFF");
  
  ethnichbox = IupHbox(ethnictxt, ethnicunktgl, NULL); //<-Enclose
  
  csexlab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(csexlab, "TITLE", "(Chromosonal) Sex:");
  
  csexmopt = IupToggle("Male\n(XY, XXY, XXXY, etc.)", NULL);
  IupSetAttribute(csexopt, "NAME, "CSEX_MALE_TGL");
  IupSetAttribute(csexopt, "VALUE, "OFF");
  
  csexfopt = IupToggle("Female\n(XX, XXX, XXXX, etc.)", NULL);
  IupSetAttribute(csexfopt, "NAME, "CSEX_FEMALE_TGL");
  IupSetAttribute(csexfopt, "VALUE, "OFF");
  
  csexunkopt = IupToggle("Unknown", NULL);
  IupSetAttribute(csexunkopt, "NAME, "CSEX_UNKNOWN_TGL");
  IupSetAttribute(csexunkopt, "VALUE, "ON");
  
  csexhbox = IupHbox(csexmopt, csexfopt, csexunkopt, NULL);
  csexrbox = IupRadio(csexhbox); //<-Enclose
  IupSetAttributeHandle(csexrbox, "VALUE_HANDLE", csexunkopt);
  
  genlab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(genlab, "TITLE", "Gender");
  
  gentxt = IupText(NULL);
  IupSetAttribute(gentxt, "NAME", "GENDER_TXT");
  IupSetAttribute(gentxt, "EXPAND", "HORIZONTAL");
  
  genunktgl = IupToggle("Unknown Gender", NULL);
  IupSetAttribute(genunktgl, "NAME, "GENDER_UNKNOWN_TGL");
  IupSetAttribute(genunktgl, "VALUE, "OFF");
  
  genhbox = IupHbox(gentxt, genunktgl, NULL); //<-Enclose
  
  notelab = IupLabel(NULL); //<-Enclose
  IupSetAttribute(notelab, "TITLE", "Notes:");
  
  noteprevlab = IupLabel(NULL);
  IupSetAttribute(noteprevlab, "TITLE", "(Empty!)");
  IupSetAttribute(noteprevlab, "NAME", "NOTES_PREVIEW_LAB");
  IupSetAttribute(noteprevlab, "EXPAND", "HORIZONTAL");
  
  noteeditbtn = IupButton("Edit Notes...", NULL);
  IupSetAttribute(noteeditbtn, "NAME", "NOTES_EDIT_BTN");
  
  notehbox = IupHbox(noteeditbtn, noteprevlab, NULL); //<-Enclose
  
  brellab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(brellab, "TITLE", "Born of Relationship:");
  
  brelchoosebtn = IupButton("Choose Relationship...", NULL);
  IupSetAttribute(brelchoosebtn, "NAME", "BIRTH_RELATIONSHIP_CHOOSE_BTN");
  
  brelprevlab = IupLabel(NULL);
  IupSetAttribute(brelprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(brelprevlab, "NAME", "BIRTH_RELATIONSHIP_PREVIEW_LAB");
  IupSetAttribute(brelprevlab, "EXPAND", "HORIZONTAL");
  
  brelhbox = hbox(brelchoosebtn, brelprevlab, NULL); //<--Enclose
  
  bdatelab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(bdatelab, "TITLE", "Date of Birth:");
  
  bdateprevlab = IupLabel(NULL);
  IupSetAttribute(bdateprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(bdateprevlab, "NAME", "BIRTH_DATE_PREVIEW_LAB");
  IupSetAttribute(bdateprevlab, "EXPAND", "HORIZONTAL");
  
  bdateeditbtn = IupButton("Edit Date...", NULL);
  IupSetAttribute(bdateeditbtn, "NAME", "BIRTH_DATE_EDIT_BTN");
  
  bdatehbox = IupHbox(bdateeditbtn, bdateprevlab, NULL); //<--Enclose
  
  bplacelab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(bplacelab, "TITLE", "Place of Birth");
  
  bplacetxt = IupText(NULL);
  IupSetAttribute(bplacetxt, "NAME", "BIRTH_PLACE_TXT");
  IupSetAttribute(bplacetxt, "EXPAND", "HORIZONTAL");
  
  bplaceunktgl = IupToggle("Unknown Place of Birth", NULL);
  IupSetAttribute(bplaceunktgl, "NAME, "BIRTH_PLACE_UNKNOWN_TGL");
  IupSetAttribute(bplaceunktgl, "VALUE, "OFF");
  
  bplacehbox = IupHbox(bplacetxt, bplaceunktgl, NULL); //<--Enclose
  
  baddrlab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(baddrlab, "TITLE", "Address of Birthplace:");
  
  baddrprevlab = IupLabel(NULL);
  IupSetAttribute(baddrprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(baddrprevlab, "NAME", "BIRTH_ADDRESS_PREVIEW_LAB");
  IupSetAttribute(baddrprevlab, "EXPAND", "HORIZONTAL");
  
  baddreditbtn = IupButton("Edit Address...", NULL);
  IupSetAttribute(baddreditbtn, "NAME", "BIRTH_ADDRESS_EDIT_BTN");
  
  baddrnatgl = IupToggle("Birthplace Has No Address", NULL);
  IupSetAttribute(baddrnatgl, "NAME, "BIRTH_ADDRESS_NA_TGL");
  IupSetAttribute(baddrnatgl, "VALUE, "OFF");
  
  baddrhbox = IupHbox(baddreditbtn, baddrprevlab, baddrnatgl, NULL); //<--Enclose
  
  bvbox = IupVbox(brellab, brelhbox, bdatelab, bdatehbox, bplacelab, bplacehbox, baddrlab, baddrhbox, NULL);
  IupSetAttribute(bvbox, "MARGIN", "10x5");
  IupSetAttribute(bvbox, "GAP", "5");
  bfrm = IupFrame(bvbox); //<-Enclose
  IupSetAttribute(bfrm, "TITLE", "Birth Details:");
  
  dalivetgl = IupToggle("Person is &Alive", NULL); //<-Enclose
  IupSetAttribute(dalivetgl, "NAME, "DEATH_ALIVE_TGL");
  IupSetAttribute(dalivetgl, "VALUE, "OFF");
  
  ddatelab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(ddatelab, "TITLE", "Date of Death:");
  
  ddateprevlab = IupLabel(NULL);
  IupSetAttribute(ddateprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(ddateprevlab, "NAME", "DEATH_DATE_PREVIEW_LAB");
  IupSetAttribute(ddateprevlab, "EXPAND", "HORIZONTAL");
  
  ddateeditbtn = IupButton("Edit Date...", NULL);
  IupSetAttribute(ddateeditbtn, "NAME", "DEATH_DATE_EDIT_BTN");
  
  ddatehbox = IupHbox(ddateeditbtn, ddateprevlab, NULL); //<--Enclose
  
  dplacelab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(dplacelab, "TITLE", "Place of Death");
  
  dplacetxt = IupText(NULL);
  IupSetAttribute(dplacetxt, "NAME", "DEATH_PLACE_TXT");
  IupSetAttribute(dplacetxt, "EXPAND", "HORIZONTAL");
  
  dplaceunktgl = IupToggle("Unknown Place of Death", NULL);
  IupSetAttribute(dplaceunktgl, "NAME, "DEATH_PLACE_UNKNOWN_TGL");
  IupSetAttribute(dplaceunktgl, "VALUE, "OFF");
  
  dplacehbox = IupHbox(dplacetxt, dplaceunktgl, NULL); //<--Enclose
  
  daddrlab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(daddrlab, "TITLE", "Address of Deathplace:");
  
  daddrprevlab = IupLabel(NULL);
  IupSetAttribute(daddrprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(daddrprevlab, "NAME", "DEATH_ADDRESS_PREVIEW_LAB");
  IupSetAttribute(daddrprevlab, "EXPAND", "HORIZONTAL");
  
  daddreditbtn = IupButton("Edit Address...", NULL);
  IupSetAttribute(daddreditbtn, "NAME", "DEATH_ADDRESS_EDIT_BTN");
  
  daddrnatgl = IupToggle("Deathplace Has No Address", NULL);
  IupSetAttribute(daddrnatgl, "NAME, "DEATH_ADDRESS_NA_TGL");
  IupSetAttribute(daddrnatgl, "VALUE, "OFF");
  
  daddrhbox = IupHbox(daddreditbtn, daddrprevlab, daddrnatgl, NULL); //<--Enclose
  
  dagelab = IupLabel(NULL); //<--Enclose
  IupSetAttribute(dagelab, "TITLE", "Age at Death:");
  
  dageprevlab = IupLabel(NULL);
  IupSetAttribute(dageprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(dageprevlab, "NAME", "DEATH_AGE_PREVIEW_LAB");
  IupSetAttribute(dageprevlab, "EXPAND", "HORIZONTAL");
  
  dageeditbtn = IupButton("Edit Age...", NULL);
  IupSetAttribute(dageeditbtn, "NAME", "DEATH_AGE_EDIT_BTN");
  
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
  IupSetAttribute(srcchoosebtn, "NAME", "SOURCE_CHOOSE_BTN");
  
  srcprevlab = IupLabel(NULL);
  IupSetAttribute(srcprevlab, "TITLE", "(Not Set!)");
  IupSetAttribute(srcprevlab, "NAME", "SOURCE_PREVIEW_LAB");
  IupSetAttribute(srcprevlab, "EXPAND", "HORIZONTAL");
  
  srchbox = hbox(srcchoosebtn, srcprevlab, NULL); //<-Enclose
  
  okbtn = IupButton("&OK", NULL);
  IupSetAttribute(okbtn, "NAME", "OK_BTN");
  IupSetAttribute(okbtn, "ACTIVE", "NO");
  IupSetAttribute(okbtn, "PADDING", "10x2");
  
  cancelbtn = IupButton("&Cancel", NULL);
  IupSetAttribute(cancelbtn, "NAME", "CANCEL_BTN");
  IupSetAttribute(cancelbtn, "PADDING", "10x2");
  
  helpbtn = IupButton("&Help...", NULL);
  IupSetAttribute(helpbtn, "NAME", "HELP_BTN");
  IupSetAttribute(helpbtn, "PADDING", "10x2");
  
  hbox1 = IupHbox(IupFill(), okbtn, cancelbtn, helpbtn, NULL); //<-Enclose
  
  vbox = IupVbox(biolab, biohbox, ethniclab, ethnichbox, csexlab, csexrbox, genlab, genhbox, notelab, notehbox, bfrm, dalivetgl, dfrm, srclab, srchbox, hbox1, NULL);
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
  
  /* TODO: Finish this! */
}

