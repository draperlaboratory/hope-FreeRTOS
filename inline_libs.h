/* To save rules in the compartmentalization policy, we'll use these implementations
 * of stateless library calls that will inline instead of the defaults. 
 * In the webserver implementation, I got this many rules resulting from each
 * lib call:
 * memcpy (50)
 * strlen (28)
 * memmov (22)
 * memcmp (15)
 * memset (12)
 * strcpy (10)
 * strncpy (8)
 */

#ifndef INLINE_LIBS
#define INLINE_LIBS
#include <stddef.h>

// **** Inline implementation of memcpy **** 
inline void *
__attribute__((always_inline))
memcpy_inline(void * dst0, const void * src0, size_t len0){
  char *dst = (char *) dst0;
  char *src = (char *) src0;
  void *save = dst0;
  while (len0--)
    {
      *dst++ = *src++;
    }
  return save;
}

// **** Inline implementation of strlen ****
inline size_t
__attribute__((always_inline))
strlen_inline(const char *s) {
    size_t i;
    for (i = 0; s[i] != '\0'; i++) ;
    return i;
}

// **** Inline implementation of strcpy ****
inline void
__attribute__((always_inline))
strcpy_inline(char * dest, const char * source) 
{
    int i = 0;
    while ((dest[i] = source[i]) != '\0')
    {
        i++;
    } 
}

// **** Inline implementation of memcmp ****
inline int
__attribute__((always_inline))
memcmp_inline(const void* s1, const void* s2,size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;
    while(n--)
        if( *p1 != *p2 )
            return *p1 - *p2;
        else
            p1++,p2++;
    return 0;
}

#endif
