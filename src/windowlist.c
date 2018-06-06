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
#include "types.h"

#include "windowlist.h"

int initwindowlist(char **windowlist)
{
  if (!windowlist) return 0;
  *windowlist = (char *) malloc(sizeof(char));
  if (!(*windowlist)) return 0;
  (*windowlist)[0] = 0;
  return 1;
}

int freewindowlist(char **windowlist)
{
  if (!windowlist || !(*windowlist)) return 0;
  free(*windowlist);
  return 1;
}

int addwindowtolist(char **windowlist, char *windowname)
{
  if (!windowlist || !(*windowlist) || !windowname) return 0;
  unsigned long wllen = 0, wnlen = 0;
  wllen = strlen(*windowlist);
  wnlen = strlen(windowname);
  if (!wnlen) return 0;
  char *newwl = (char *) malloc(sizeof(char) * (3+wllen+wnlen));
  if (!newwl) return 0;
  if (wllen)
  {
    strcpy(newwl, *windowlist);
    strcat(newwl, "\n");
  }
  else newwl[0] = 0;
  strcat(newwl, windowname);
  free(*windowlist);
  *windowlist = newwl;
  return 1;
}
