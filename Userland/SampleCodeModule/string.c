// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <string.h>

uint8_t strcmp (const char *p1, const char *p2) {
  const unsigned char *s1 = (const unsigned char *) p1;
  const unsigned char *s2 = (const unsigned char *) p2;
  unsigned char c1, c2;
  do {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0')
        return c1 - c2;
    } while (c1 == c2);
  return c1 - c2;
}

uint8_t strlen(const char *str) {
	const char *s;
	for (s = str; *s; ++s);
	return (s - str);
}

char * strcpy(char *strDest, const char *strSrc)
{
   if(strDest == 0 || strSrc == 0 )
    return (char*) 0 ;   

    char *temp = strDest;
    while((*strDest++ = *strSrc++)); 
    return temp;
}


int strcat(const char* src, char* dest){
  int i = 0, j = 0;
  while(*(dest+i)) i++;
  while(*(src+j)) dest[i++] = src[j++];
  dest[i] = 0;
  return i;
}

int charcat(char c, char* dest){
  int i = 0;
  while(*(dest+i)) i++;
  dest[i++] = c;
  dest[i] = 0; 
  return i;
}
