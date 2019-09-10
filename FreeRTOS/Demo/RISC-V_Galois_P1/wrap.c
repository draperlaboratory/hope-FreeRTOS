#include <string.h>
#include <stdint.h>
#include <stdio.h>

void *__wrap_memcpy( void *pvDest, const void *pvSource, size_t ulBytes )
{
  uint32_t *pcDest = ( uint32_t * ) pvDest;
  uint32_t *pcSource = ( uint32_t * ) pvSource;
  size_t x;

  if ( ulBytes % sizeof(uint32_t) ) {
    ulBytes += sizeof(uint32_t);
  }

  ulBytes /= sizeof(uint32_t);

  for( x = 0; x < ulBytes; x++ )
  {
    *pcDest = *pcSource;
    pcDest++;
    pcSource++;
  }

  return pvDest;
}

void *__wrap_memset( void *pvDest, int iValue, size_t ulBytes )
{
  uint32_t *pcDest = ( uint32_t * ) pvDest;
  size_t x;

  if ( ulBytes % sizeof(uint32_t) ) {
    ulBytes += sizeof(uint32_t);
  }

  ulBytes /= sizeof(uint32_t);

  for( x = 0; x < ulBytes; x++ )
  {
    *pcDest = (uint32_t) iValue;
    pcDest++;
  }

  return pvDest;
}

int __wrap_printf(const char *s, ...)
{
  char buf[256];
  va_list vl;

  const char *p = &buf[0];

  va_start(vl, s);
  vsnprintf(buf, sizeof buf, s, vl);
  va_end(vl);

  puts(p);

  return 0;
}

int __wrap_puts(const char *s)
{
  while (*s != '\0') {
    uart0_txchar(*s);

    if (*s == '\n') {
      uart0_txchar('\r');
    }

    ++s;
  }

  return 0;
}
