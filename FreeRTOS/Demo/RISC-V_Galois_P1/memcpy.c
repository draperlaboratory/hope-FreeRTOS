#include <string.h>
#include <stdint.h>

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
