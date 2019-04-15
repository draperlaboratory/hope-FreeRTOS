/*
 * File: cgi-util.c
 *
 * Description:
 *   FreeRTOS CGI port of the cgi-util functions.
 *
 * Copyright DornerWorks 2018
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1.  Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DORNERWORKS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdarg.h>
#include <stdio.h>
#include "cgi-util.h"

CGI_HEADER_FUNCTION(SimpleCookie, char *name, char *value)
{
    snprintf(pcHeaderBuffer, xHeaderBufferLen,
             "%sSet-Cookie: %s=%s;\r\n", pcHeaderBuffer, name, value);
}

CGI_FUNCTION(Whitespace, size_t size)
{
  CGI_PRINTF("<p>&nbsp;\n");
}

CGI_FUNCTION(FormStart, form_type_t form_type, const char *destination)
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

CGI_FUNCTION(FormEnd)
{
	CGI_PRINTF("</form>\n");
}

CGI_FUNCTION(LinkButton, char *text, char *destination, ...)
{
  char buffer[0x1000];
  va_list args;

  va_start(args, destination);
  vsnprintf(buffer, sizeof(buffer), destination, args);
  va_end(args);
  
  CGI_PRINTF("<form method=\"GET\" action=\"%s\">\n", buffer);
	CGI_PRINTF("<input type=\"submit\" value=\"%s\"><\form>\n", text);
}

CGI_FUNCTION(HtmlEscape, char *text)
{
  char *p;
  size_t len;
  size_t i;

  len = strlen(text);
  p = pcWriteBuffer;

  for(i = 0; i < len; i++) {
    switch(text[i]) {
      case '<':
        snprintf(p, xWriteBufferLen, "&lt;");
        p += 4;
        break;
      case '&':
        snprintf(p, xWriteBufferLen, "&amp;");
        p += 5;
        break;
      case '>':
        snprintf(p, xWriteBufferLen, "&gt;");
        p += 4;
        break;
      default:
        *p = text[i];
        p++;
    }
  }
}
