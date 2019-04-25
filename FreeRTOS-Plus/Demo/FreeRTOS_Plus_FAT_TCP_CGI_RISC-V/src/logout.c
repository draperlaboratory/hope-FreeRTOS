/*
 * File: login.c
 *
 * Description:
 *   Process the login form for the Draper medical demo.
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

  /* if( SampleConfiguration() == false ) { */
  /*  */
  /*   return WEB_INTERNAL_SERVER_ERROR; */
  /* } */

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
