#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iup/iup.h>
#include <iup/iup_config.h>
#include <errno.h>
#include "safeiupconf.h"
#include "utf8.h"
#include "proginfo.h"

int main (int argc, char *argv[])
{
  Ihandle *config;
  
  IupOpen(&argc, &argv);
  config = IupConfig();
  IupSetAttribute(config, "APP_NAME", PROG_NAME);
  IupSetAttribute(config, "APP_CONFIG", "NO");
  SafeIupConfigLoad(config);
  
  IupClose();
  return EXIT_SUCCESS;
}
