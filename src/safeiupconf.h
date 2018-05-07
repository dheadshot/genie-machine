#ifndef __ICONHLP_H__

#define __ICONHLP_H__

#include <iup/iup.h>
#include <iup/iup_config.h>

int afnexists(const char *afn);
char* iConfigSetFilename_(Ihandle* ih);
int SafeIupConfigLoad(Ihandle *config);

#endif
