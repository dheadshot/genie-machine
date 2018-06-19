#ifndef __UTF8_H__

#define __UTF8_H__

int numbytesinchar(const char firstbyte);
long strcharcount(const char *str);
int hasnonwhitespace(const char *astr);
int str_compare(const char *l, const char *r, long llen, long rlen, int casesensitive);
int str_compare_nt(const char *l, const char *r, int casesensitive);
int str_comparet(const char *l, const char *r, long llen, long rlen, int casesensitive);
int str_comparet_nt(const char *l, const char *r, int casesensitive);
long *str_find(const char *str, const char *str_to_find, int casesensitive);
long bytecountchars(const char *ctext, long ccount);
char *utf8char(unsigned long utf32char);
unsigned long utf8offset(const char *utf8str);
unsigned long sstrlen(const char *a); /* Safe StrLen */
unsigned long strcpy_count(char *outstr, char *instr, unsigned long numchars);

#endif
