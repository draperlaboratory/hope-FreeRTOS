#ifndef CGI_UTIL_H
#define CGI_UTIL_H

#include <string.h>

#include "user.h"
#include "auth.h"

#define CGI_SESSION_COOKIE_SIZE AUTH_SESSION_ID_SIZE
#define SECONDS_IN_DAY 86400

#define __CGI_CALL(__buffer_type_, __name, ...) \
  __name(pc##__buffer_type_##Buffer, x##__buffer_type_##BufferLen, ##__VA_ARGS__)

#define __CGI_IMPL_CALL(__buffer_type_, __name, ...) \
  __name(&pc##__buffer_type_##Buffer, &x##__buffer_type_##BufferLen, ##__VA_ARGS__)

#define CGI_CALL(__name, ...) __CGI_CALL(Write, __name, ##__VA_ARGS__)
#define CGI_HEADER_CALL(__name, ...) __CGI_CALL(Header, __name, ##__VA_ARGS__)

#define CGI_IMPL_CALL(__name, ...) __CGI_IMPL_CALL(Write, __name, ##__VA_ARGS__)
#define CGI_IMPL_HEADER_CALL(__name, ...) __CGI_IMPL_CALL(Header, __name, ##__VA_ARGS__)

#define __CGI_FUNCTION(__buffer_type_, __type, __name, ...) \
  __type __name(char **pc##__buffer_type_##Buffer, size_t *x##__buffer_type_##BufferLen, ##__VA_ARGS__)

#define CGI_FUNCTION(__type, __name, ...) __CGI_FUNCTION(Write, __type, __name, ##__VA_ARGS__)
#define CGI_HEADER_FUNCTION(__type, __name, ...) __CGI_FUNCTION(Header, __type, __name, ##__VA_ARGS__) 

#define CGI_PRINTF(__fmt, ...) \
  do { \
    size_t __print_len = snprintf(*pcWriteBuffer, *xWriteBufferLen, __fmt, ##__VA_ARGS__); \
    *pcWriteBuffer += __print_len; \
    *xWriteBufferLen -= __print_len; \
  } while(0)

typedef enum {
  CGI_FORM_GET,
  CGI_FORM_POST,
} form_type_t;

bool GetActiveUser(user_t **user, bool mock);
user_t *GetSessionUser(void);

CGI_HEADER_FUNCTION(void, SetCookie, char *name, char *value);
CGI_HEADER_FUNCTION(void, DeleteCookie, char *name);

CGI_FUNCTION(void, FormStart, form_type_t form_type, const char *destination);
CGI_FUNCTION(void, FormEnd);
CGI_FUNCTION(void, Whitespace, size_t size);
CGI_FUNCTION(void, LinkButton, char *text, char *destination, ...);
CGI_FUNCTION(void, HtmlEscape, char *text);

#endif // CGI_UTIL_H
