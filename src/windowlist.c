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

int initwindowlist(char **windowlistvar)
{
  if (!windowlistvar) return 0;
  *windowlistvar = (char *) malloc(sizeof(char));
  if (!(*windowlistvar)) return 0;
  (*windowlistvar)[0] = 0;
  return 1;
}

int freewindowlist(char **windowlistvar)
{
  if (!windowlistvar || !(*windowlistvar)) return 0;
  free(*windowlistvar);
  (*windowlistvar) = NULL;
  return 1;
}

int addwindowtolist(char **windowlistvar, char *windowname)
{
  if (!windowlistvar || !(*windowlistvar) || !windowname) return 0;
  unsigned long wllen = 0, wnlen = 0;
  wllen = strlen(*windowlistvar);
  wnlen = strlen(windowname);
  if (!wnlen) return 0;
  char *newwl = (char *) malloc(sizeof(char) * (3+wllen+wnlen));
  if (!newwl) return 0;
  if (wllen)
  {
    strcpy(newwl, *windowlistvar);
    strcat(newwl, "\n");
  }
  else newwl[0] = 0;
  strcat(newwl, windowname);
  free(*windowlistvar);
  *windowlistvar = newwl;
  return 1;
}

int duplicatewindowlist(char **newwindowlistvar, char *windowlist)
{
  if (!newwindowlistvar || !windowlist) return 0;
  *newwindowlistvar = (char *) malloc(sizeof(char)*(1+strlen(windowlist)));
  if (!(*newwindowlistvar)) return 0;
  strcpy(*newwindowlistvar, windowlist);
  return 1;
}

int findwindowinlist(char *windowlist, char *windowname)
/* Returns the window's position (index 0) in the list, or -1 if it isn't there. */
{
  unsigned long i = 0, j = 0, wnlen = strlen(windowname);
  int n = 0;
  for (i = 0; windowlist[i] != 0; i++)
  {
    if (windowlist[i] == '\n')
    {
      if (str_comparet(windowlist + (sizeof(char)+j),windowname,i-j, wnlen, 1) == 1) return n;
      n++;
      j = i + 1;
    }
  }
  if (str_comparet(windowlist + (sizeof(char)+j),windowname,i-j, wnlen, 1) == 1) return n;
  return -1;
}
