#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

int numbytesinchar(const char firstbyte)
{
  if ((firstbyte & 0x80) == 0) return 1;
  if ((firstbyte & 0x40) == 0) return -1; /* Not a first byte! */
  if ((firstbyte & 0x20) == 0) return 2;
  if ((firstbyte & 0x10) == 0) return 3;
  if ((firstbyte & 0x08) == 0) return 4;
  return -2; /* Unsupported UTF-8 Extension! */
}

long strcharcount(const char *str)
{
  long i = 0, n, chi = 0, len;
  len = strlen(str);
  while (i<len)
  {
    n = (long) numbytesinchar(str[i]);
    if (n<0) n=1;
    chi++;
    i += n;
  }
  return chi;
}

int hasnonwhitespace(const char *astr)
{
  int ans = 0, n;
  while (astr[0])
  {
    n = numbytesinchar(astr[0]);
    if (n<1) n=1;
    if (n == 1 && astr[0] < 0x21)
    {
      astr += n;
    }
    else
    {
      ans = 1;
      break;
    }
  }
  return ans;
}

int str_compare(const char *l, const char *r, long llen, long rlen, int casesensitive)
{
  if (!l || !r || !rlen) return 0;
  long lj = 0, rj = 0;
  while (lj<llen && rj < rlen)
  {
    int diff, i, n, m;
    char l_char[5], r_char[5];
    memset(l_char,0,sizeof(char)*5);
    memset(r_char,0,sizeof(char)*5);
    n = numbytesinchar(l[lj]);
    if (n<0) { /*fprintf(stderr,"nbl=%d  ",n);*/ n = 1; }
    /*if (n>1) fprintf(stderr,"--nbl=%d  ",n);*/
    for (i=0;i<n;i++)
    {
      l_char[i] = l[lj];
      lj++;
    }
    l_char[n] = 0;
    
    m = numbytesinchar(r[rj]);
    if (m<0) { /*fprintf(stderr,"nbr=%d  ",m);*/ m = 1; }
    /*if (m>1) fprintf(stderr,"--nbr=%d  ",m);*/
    for (i=0;i<m;i++)
    {
      r_char[i] = r[rj];
      rj++;
    }
    r_char[m] = 0;
    
    if (m == n)
    {
      for (i=0;i<n;i++)
      {
        if (l_char[0] == 0 || r_char[0] == 0) return 2;
        if (casesensitive || i>0 || (l_char[i] & 0x80) != 0 || (r_char[i] & 0x80) != 0)
        {
          diff = ((int) l_char[i]) - ((int) r_char[i]);
        }
        else
        {
          diff = ((int) tolower((int) l_char[0])) - ((int) tolower((int) r_char[0]));
        }
        if (diff != 0) break;
      }
    }
    else diff = 0x100;
    if (diff != 0) return 0;
  }
  if (l[lj] == r[rj]) return 1;
  if (l[lj] == 0 || r[rj] == 0) return 1;
  return 0;
}

int str_compare_nt(const char *l, const char *r, int casesensitive)
{
  long llen, rlen;
  if (!l || !r) return 0;
  llen = strlen(l);
  rlen = strlen(r);
  return str_compare(l, r, llen, rlen, casesensitive);
}

int str_comparet(const char *l, const char *r, long llen, long rlen, int casesensitive)
{
  if (llen != rlen) return 0;
  return str_compare(l, r, llen, rlen, casesensitive);
}

int str_comparet_nt(const char *l, const char *r, int casesensitive)
{
  long llen, rlen;
  if (!l || !r) return 0;
  llen = strlen(l);
  rlen = strlen(r);
  return str_comparet(l, r, llen, rlen, casesensitive);
}


long *str_find(const char *str, const char *str_to_find, int casesensitive)
{
  long i, chi, str_len, str_to_find_len, t_str_len, count, *ret;
  int n, sc;
  char a;
  ret = (long *) malloc(sizeof(long)*3);
  if (ret == NULL) return 0;
  memset(ret,0,sizeof(long)*3);
  
  if (!str || str[0] == 0 || !str_to_find || str_to_find[0] == 0)
  {
    ret[0] = -2;
    ret[1] = -2;
    return ret;
  }
  
  str_len = (long) strlen(str);
  str_to_find_len = (long) strlen(str_to_find);
  	/*fprintf(stderr,"--stf:'%s'(%d)\n",str_to_find,str_to_find_len);*/
  count = str_len - str_to_find_len;
  if (count <0)
  {
    ret[0] = -2;
    ret[1] = -2;
    return ret;
  }
  count++;
  
  chi = 0;
  i = 0;
  t_str_len = (long) strlen(str);
  while (i<count)
  {
    a = *str;
    n = numbytesinchar(a);
    if (n<0) n=1;
    sc = str_compare(str, str_to_find, t_str_len, str_to_find_len, casesensitive);
    if (sc)
    {
      /*fprintf(stderr,"sc=%d\n",sc);*/
      ret[0] = i;
      ret[1] = chi;
      return ret;
    }
    chi++;
    i+=n;
    str+=n;
    t_str_len -= n;
  }
  ret[0] = -1;
  ret[1] = -1;
  return ret;
}

long bytecountchars(const char *ctext, long ccount)
{
  long cnum = 0, bnum = 0;
  char a[5] = {0,0,0,0,0};
  while (cnum < ccount)
  {
    a[0] = ctext[bnum];
    a[1] = 0;
    a[2] = 0;
    a[3] = 0;
    if ((a[0] & 0x80) == 0)
    {
      /* ASCII */
      cnum++;
    }
    else if ((a[0] & 0x40) == 0) return -1; /* bad utf8! */
    else if ((a[0] & 0x20) == 0)
    {
      /* 0x0080 to 0x07FF */
      bnum++;
      a[1] = ctext[bnum];
      if ((a[1] & 0x80) == 0 || (a[1] & 0x40) != 0) return -1; /* bad utf8! */
      cnum++;
    }
    else if ((a[0] & 0x10) == 0)
    {
      /* 0x0800 to 0xFFFF */
      bnum++;
      a[1] = ctext[bnum];
      if ((a[1] & 0x80) == 0 || (a[1] & 0x40) != 0) return -1; /* bad utf8! */
      bnum++;
      a[2] = ctext[bnum];
      if ((a[2] & 0x80) == 0 || (a[2] & 0x40) != 0) return -1; /* bad utf8! */
      cnum++;
    }
    else if ((a[0] & 0x8) == 0)
    {
      /* 0x010000 to 0x10FFFF */
      bnum++;
      a[1] = ctext[bnum];
      if ((a[1] & 0x80) == 0 || (a[1] & 0x40) != 0) return -1; /* bad utf8! */
      bnum++;
      a[2] = ctext[bnum];
      if ((a[2] & 0x80) == 0 || (a[2] & 0x40) != 0) return -1; /* bad utf8! */
      bnum++;
      a[3] = ctext[bnum];
      if ((a[3] & 0x80) == 0 || (a[3] & 0x40) != 0) return -1; /* bad utf8! */
      cnum++;
    }
    else
    {
      fprintf(stderr,"Unsupported char %d ('%c'=0x%X=0%o)!\n",a[0], a[0], a[0], a[0]);
      return -2; /* Unsupported variant of UTF8! */
    }
    bnum++;
  }
  
  return bnum;
}

char *utf8char(unsigned long utf32char)
{
  char *utf8c;
  if (utf32char < 0x80)
  {
    utf8c = (char *) malloc(sizeof(char)*2);
    if (utf8c == NULL) return NULL;
    utf8c[0] = (char) utf32char;
    utf8c[1] = 0;
  }
  else if (utf32char < 0x800)
  {
    utf8c = (char *) malloc(sizeof(char)*3);
    if (utf8c == NULL) return NULL;
    utf8c[0] = ((char) (0xC0 | ((utf32char >> 6) & 0x1F)));
    utf8c[1] = ((char) (0x80 | ( utf32char       & 0x3F)));
    utf8c[2] = 0;
  }
  else if (utf32char < 0x10000)
  {
    utf8c = (char *) malloc(sizeof(char)*4);
    if (utf8c == NULL) return NULL;
    utf8c[0] = ((char) (0xE0 | ((utf32char >> 12) & 0xF)));
    utf8c[1] = ((char) (0x80 | ((utf32char >>  6) & 0x3F)));
    utf8c[2] = ((char) (0x80 | ( utf32char        & 0x3F)));
    utf8c[3] = 0;
  }
  else if (utf32char < 0x110000)
  {
    utf8c = (char *) malloc(sizeof(char)*5);
    if (utf8c == NULL) return NULL;
    utf8c[0] = ((char) (0xF0 | ((utf32char >> 18) & 0x7)));
    utf8c[1] = ((char) (0x80 | ((utf32char >> 12) & 0x3F)));
    utf8c[2] = ((char) (0x80 | ((utf32char >>  6) & 0x3F)));
    utf8c[3] = ((char) (0x80 | ( utf32char        & 0x3F)));
    utf8c[4] = 0;
  }
  else return NULL;
  return utf8c;
}

unsigned long utf8offset(const char *utf8str)
{
  int n = numbytesinchar(utf8str[0]);
  unsigned long utf32o = 0;
  switch (n)
  {
    case 1:
      utf32o = (unsigned long) utf8str[0];
    break;
    
    case 2:
      utf32o =  ((unsigned long)  (utf8str[1] & 0x3F));
      utf32o |= ((unsigned long) ((utf8str[0] & 0x1F) << 6));
    break;
    
    case 3:
      utf32o =  ((unsigned long)  (utf8str[2] & 0x3F));
      utf32o |= ((unsigned long) ((utf8str[1] & 0x3F) << 6));
      utf32o |= ((unsigned long) ((utf8str[0] & 0xF ) << 12));
    break;
    
    case 4:
      utf32o =  ((unsigned long)  (utf8str[3] & 0x3F));
      utf32o |= ((unsigned long) ((utf8str[2] & 0x3F) << 6));
      utf32o |= ((unsigned long) ((utf8str[2] & 0x3F) << 12));
      utf32o |= ((unsigned long) ((utf8str[0] & 0x7 ) << 18));
    break;
    
    default:
      utf32o = 0xFFFD;
    break;
  }
  return utf32o;
}
