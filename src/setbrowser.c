#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iup/iup.h>
#include <iup/iup_config.h>
#include <errno.h>

#include "utf8.h"
#include "doerror.h"

#include "setbrowser.h"

int setbrowser_test_action_cb(Ihandle *ih)
{
  Ihandle *txt = IupGetDialogChild(ih, "PRGCOMMAND_TXT");
  Ihandle *okbtn = IupGetDialogChild(ih, "OK_BTN");
  Ihandle *dlg = IupGetDialog(ih);
  char *str = IupGetAttribute(txt, "VALUE");
  char *acmd;
  
  if (!str || !str[0] || !hasnonwhitespace(str))
  {
    show_error("You must enter a command!", 1, NULL, dlg);
  }
  else
  {
    acmd = (char *) malloc(sizeof(char)*(1+strlen(str)));
    strcpy(acmd, str);
    fprintf(stderr, "Trying \"%s\"...\n",acmd);
    if (IupExecute(acmd, "") > 0) //No, You CAN'T use NULL here, contrary to the documentation!
    {
      IupSetAttribute(okbtn, "ACTIVE", "YES");
    }
    else
    {
      show_error("Failed to open browser using command!", 0, NULL, dlg); //Warning
    }
    free(acmd);
  }
  return IUP_DEFAULT;
}

int setbrowser_txt_valuechanged_cb(Ihandle *ih)
{
  Ihandle *okbtn = IupGetDialogChild(ih, "OK_BTN");
  IupSetAttribute(okbtn, "ACTIVE", "NO");
  return IUP_DEFAULT;
}

int setbrowser_ok_action_cb(Ihandle *ih)
{
  Ihandle *txt = IupGetDialogChild(ih, "PRGCOMMAND_TXT");
  Ihandle *dlg = IupGetDialog(ih);
  char *str = IupGetAttribute(txt, "VALUE");
  
  if (!str || !str[0]) show_error("You must enter a command!", 1, NULL, dlg);
  else if (IupGetInt(ih, "ACTIVE") == 0)
  {
    show_error("You must test the command first!", 1, NULL, dlg);
  }
  else
  {
    IupSetAttribute(dlg, "STATUS", "1");
    return IUP_CLOSE;
  }
  return IUP_DEFAULT;
}

int setbrowser_cancel_action_cb(Ihandle *ih)
{
  IupSetAttribute(IupGetDialog(ih), "STATUS", "0");
  return IUP_CLOSE;
}

int dosetbrowser(Ihandle *parentdlg, Ihandle *config)
{
  Ihandle *sbdlg, *vbox, *hbox1, *hbox2, *okbtn, *cancelbtn, *testbtn, *txt, 
          *lbl1, *lbl2;
  char *curhelpapp = IupGetAttribute(NULL, "HELPAPP");
  int ans = 0;
  
  lbl1 = IupLabel(NULL);
  IupSetAttribute(lbl1, "TITLE", "Enter the command (program filename) of the web browser your system uses:");
  txt = IupText(NULL);
  IupSetAttribute(txt, "NAME", "PRGCOMMAND_TXT");
  if (curhelpapp)
  {
    IupSetStrAttribute(txt, "VALUE", curhelpapp);
  }
  IupSetCallback(txt, "VALUECHANGED_CB", (Icallback) setbrowser_txt_valuechanged_cb);
  IupSetAttribute(txt, "EXPAND", "HORIZONTAL");
  lbl2 = IupLabel(NULL);
  IupSetAttribute(lbl2, "TITLE", "Test the command works before pressing OK.");
  testbtn = IupButton("&Test...", NULL);
  IupSetAttribute(testbtn, "PADDING", "10x2");
  IupSetCallback(testbtn, "ACTION", (Icallback) setbrowser_test_action_cb);
  okbtn = IupButton("&OK", NULL);
  IupSetAttribute(okbtn, "NAME", "OK_BTN");
  IupSetAttribute(okbtn, "ACTIVE", "NO");
  IupSetAttribute(okbtn, "PADDING", "10x2");
  IupSetCallback(okbtn, "ACTION", (Icallback) setbrowser_ok_action_cb);
  cancelbtn = IupButton("&Cancel", NULL);
  IupSetAttribute(cancelbtn, "PADDING", "10x2");
  IupSetCallback(cancelbtn, "ACTION", (Icallback) setbrowser_cancel_action_cb);
  
  hbox1 = IupHbox(lbl2, testbtn, NULL);
  hbox2 = IupHbox(IupFill(), okbtn, cancelbtn, NULL);
  vbox = IupVbox(lbl1, txt, hbox1, hbox2, NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "5");
  
  sbdlg = IupDialog(vbox);
  IupSetAttribute(sbdlg, "TITLE", "Set Browser");
  IupSetAttribute(sbdlg, "DIALOGFRAME", "YES");
  IupSetAttributeHandle(sbdlg, "DEFAULTENTER", okbtn);
  IupSetAttributeHandle(sbdlg, "DEFAULTESC", cancelbtn);
  IupSetAttributeHandle(sbdlg, "PARENTDIALOG", parentdlg);
  IupSetCallback(sbdlg, "CLOSE_CB", (Icallback) setbrowser_cancel_action_cb);
  
  IupPopup(sbdlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
  if (IupGetInt(sbdlg, "STATUS") == 1)
  {
    char *acmd = IupGetAttribute(txt, "VALUE");
    if (acmd && acmd[0])
    {
      IupSetStrAttribute(NULL, "HELPAPP", acmd);
      IupConfigSetVariableStr(config, "MainWindow", "HelpPrg", acmd);
      ans = 1;
    }
    else
    {
      show_error("Invalid Browser Filename!", 1, NULL, parentdlg);
    }
  }
  IupDestroy(sbdlg);
  
  return ans;
}