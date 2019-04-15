#ifndef CGI_UTIL_H
#define CGI_UTIL_H

#include <string.h>

#include "user.h"
#include "auth.h"

#define CGI_SESSION_COOKIE_SIZE AUTH_SESSION_ID_SIZE
#define SECONDS_IN_DAY 86400

#define CGI_CALL(__name, ...) \
  __name(pcWriteBuffer, xWriteBufferLen, ##__VA_ARGS__)
#define CGI_FUNCTION(__name, ...) \
  void __name(char *pcWriteBuffer, size_t xWriteBufferLen, ##__VA_ARGS__)

#define CGI_HEADER_CALL(__name, ...) \
  __name(pcHeaderBuffer, xHeaderBufferLen, ##__VA_ARGS__)
#define CGI_HEADER_FUNCTION(__name, ...) \
  void __name(char *pcHeaderBuffer, size_t xHeaderBufferLen, ##__VA_ARGS__)

#define CGI_PRINTF(__fmt, ...) \
  snprintf(pcWriteBuffer, xWriteBufferLen, __fmt, ##__VA_ARGS__)

typedef enum {
  CGI_FORM_GET,
  CGI_FORM_POST,
} form_type_t;

bool GetActiveUser(user_t **user, bool mock);
user_t *GetSessionUser(void);

CGI_HEADER_FUNCTION(SimpleCookie, char *name, char *value);

CGI_FUNCTION(FormStart, form_type_t form_type, const char *destination);
CGI_FUNCTION(FormEnd);
CGI_FUNCTION(Whitespace, size_t size);
CGI_FUNCTION(LinkButton, char *text, char *destination, ...);
CGI_FUNCTION(HtmlEscape, char *text);

#endif // CGI_UTIL_H
