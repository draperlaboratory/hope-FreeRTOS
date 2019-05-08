/* Copyright © 2017-2019 The Charles Stark Draper Laboratory, Inc. and/or Dover Microsystems, Inc. */
/* All rights reserved. */

/* Use and disclosure subject to the following license. */

/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the */
/* "Software"), to deal in the Software without restriction, including */
/* without limitation the rights to use, copy, modify, merge, publish, */
/* distribute, sublicense, and/or sell copies of the Software, and to */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions: */

/* The above copyright notice and this permission notice shall be */
/* included in all copies or substantial portions of the Software. */

/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE */
/* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION */
/* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION */
/* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include <stdarg.h>
#include "cgi-util.h"

#define CGI_HEADER_LEN_MAX 256

CGI_HEADER_FUNCTION(void, SetCookie, char *name, char *value)
{
    snprintf(*pcHeaderBuffer, *xHeaderBufferLen,
             "%s=%s;\r\n", name, value);
}

CGI_HEADER_FUNCTION(void, DeleteCookie, char *name)
{
    snprintf(*pcHeaderBuffer, *xHeaderBufferLen,
             "%s=;expires=Thu, 01 Jan 1970 00:00:00 UTC;\r\n", name);
}

CGI_FUNCTION(void, Whitespace, size_t size)
{
  size_t i;

  for(i = 0; i < size; i++) {
    CGI_PRINTF("<p>&#160;</p>\n");
  }
}

CGI_FUNCTION(void, FormStart, form_type_t form_type, const char *destination)
{
  switch(form_type) {
    case CGI_FORM_GET:
      CGI_PRINTF("<form method=\"GET\" action=\"%s\">\n", destination);
      break;
    case CGI_FORM_POST:
      CGI_PRINTF("<form method=\"POST\" enctype=\"multipart/form-data\"action=\"%s\">\n", destination);
      break;
    default:
      break;
  }
}

CGI_FUNCTION(void, FormEnd)
{
	CGI_PRINTF("</form>\n");
}

CGI_FUNCTION(void, LinkButton, char *text, char *destination, ...)
{
  char buffer[0x1000];
  va_list args;

  va_start(args, destination);
  vsnprintf(buffer, sizeof(buffer), destination, args);
  va_end(args);
  
  CGI_PRINTF("<form method=\"GET\" action=\"%s\">\n", buffer);
	CGI_PRINTF("<input type=\"submit\" value=\"%s\"></input></form>\n", text);
}

CGI_FUNCTION(void, HtmlEscape, char *text)
{
  size_t len;
  size_t i;

  len = strlen(text);

  for(i = 0; i < len; i++) {
    switch(text[i]) {
      case '<':
        CGI_PRINTF("&lt;");
        break;
      case '&':
        CGI_PRINTF("&amp;");
        break;
      case '>':
        CGI_PRINTF("&gt;");
        break;
      default:
        CGI_PRINTF("%c", text[i]);
    }
  }
}
