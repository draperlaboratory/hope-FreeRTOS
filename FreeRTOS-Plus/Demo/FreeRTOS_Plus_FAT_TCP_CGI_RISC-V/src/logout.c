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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cgi-args.h"
#include "cgi-util.h"
#include "auth.h"
#include "sample.h"
#include "FreeRTOS.h"
#include "FreeRTOS_HTTP_commands.h"

CGI_FUNCTION(bool, Logout, char *session_id);

BaseType_t CgiLogout( char *pcWriteBuffer, size_t xWriteBufferLen,
    char *pcHeaderBuffer, size_t xHeaderBufferLen,
    const char *pcCgiArgs )
{
  char session_id[AUTH_SESSION_ID_SIZE] = { 0 };

  CgiArgValue(session_id, AUTH_SESSION_ID_SIZE, "sessionId", pcCgiArgs);

  CGI_IMPL_HEADER_CALL(DeleteCookie, "sessionId");

  if(CGI_IMPL_CALL(Logout, session_id) == false) {
    return WEB_INTERNAL_SERVER_ERROR;
  }

  return WEB_REPLY_OK;
}

CGI_FUNCTION(bool, Logout, char *session_id)
{
  if(AuthEndSession(session_id) == false) {
    CGI_PRINTF("Failed to end session\n");
    return false;
  }
  
  CGI_PRINTF("login.html");
  return true;
}
