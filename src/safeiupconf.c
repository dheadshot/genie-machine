#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iup/iup.h>
#include <iup/iup_config.h>
#include "safeiupconf.h"

int afnexists(const char *afn)
{
  FILE *af;
  if (!afn)
  {
    fprintf(stderr, "Filename not specified!\n");
    return -2;
  }
  af = fopen(afn, "r");
  if (af)
  {
    	fprintf(stderr, "--File '%s' Exists--\n", afn);
    fclose(af);
    return 1;
  }
  if (errno == ENOENT)
  {
    	fprintf(stderr, "--File '%s' Does Not Exist--\n", afn);
    return 0;
  }
    	fprintf(stderr, "--Error %d with File '%s'--\n", errno, afn);
  return -1;
}

char* iConfigSetFilename_(Ihandle* ih)
{
  char* app_name;
  char* app_path;
  int app_config;
  char* home;
  
  char *filename;//[10240] = "";
  filename = (char *) malloc(10240*sizeof(char));
  if (filename == NULL) return NULL;
  char* app_filename = IupGetAttribute(ih, "APP_FILENAME");
  if (app_filename)
    return app_filename;

  app_name = IupGetAttribute(ih, "APP_NAME");
  app_path = IupGetAttribute(ih, "APP_PATH");
  app_config = IupGetInt(ih, "APP_CONFIG");

  if (!app_name)
  {
    free(filename);
    return NULL;
  }

  home = getenv("HOME");
  if (home && !app_config)
  {
    /* UNIX format */
    strcat(filename, home);
    strcat(filename, "/.");
    strcat(filename, app_name);
  }
  else
  {
    /* Windows format */
    char* homedrive = getenv("HOMEDRIVE");
    char* homepath = getenv("HOMEPATH");
    if (homedrive && homepath && !app_config)
    {
      strcat(filename, homedrive);
      strcat(filename, homepath);
      strcat(filename, "\\");
      strcat(filename, app_name);
      strcat(filename, ".cfg");
    }
    else
    {
      if (!app_path)
      {
        free(filename);
        return NULL;
      }

      strcat(filename, app_path);
#ifndef WIN32
      strcat(filename, ".");
#endif
      strcat(filename, app_name);
#ifdef WIN32
      strcat(filename, ".cfg");
#endif
    }
  }
  
  IupSetStrAttribute(ih, "FILENAME", filename);
  
  char *afn =  IupGetAttribute(ih, "FILENAME");
  return afn;
}

int SafeIupConfigLoad(Ihandle *config)
{
  const char *conffn = iConfigSetFilename_(config);
  if (!conffn || !conffn[0]) return -1;
  if (afnexists(conffn) > 0)
  {
    IupConfigLoad(config); 
    return 1;
  }
  return 0;
}
