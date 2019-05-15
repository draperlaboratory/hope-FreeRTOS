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

CGI_HEADER_FUNCTION(auth_result_t, Login, char *username, char *password);
CGI_FUNCTION(BaseType_t, CheckLoginResult, auth_result_t login_result);

BaseType_t CgiLogin( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs )
{
    BaseType_t rv = WEB_REPLY_OK;
    char username[KEY_SIZE_MAX] = { 0 }; 
    char password[KEY_SIZE_MAX] = { 0 };
    auth_result_t login_result;

    CgiArgValue( username, sizeof(username), "username", pcCgiArgs );
    CgiArgValue( password, sizeof(password), "password", pcCgiArgs );

    login_result = CGI_IMPL_HEADER_CALL(Login, username, password);
    rv = CGI_IMPL_CALL(CheckLoginResult, login_result);

    return rv;
}

CGI_FUNCTION(BaseType_t, CheckLoginResult, auth_result_t login_result)
{
    switch(login_result)
    {
       case AUTH_SUCCESS:
           CGI_PRINTF("dashboard.html" );
           return WEB_REPLY_OK;
       case AUTH_USER_NOT_FOUND:
           CGI_PRINTF("User not found in database" );
           return WEB_BAD_REQUEST;
       case AUTH_INVALID_CREDENTIALS:
           CGI_PRINTF("Invalid password" );
           return WEB_BAD_REQUEST;
       case AUTH_NOT_LOGGED_IN:
           CGI_PRINTF("One or more empty fields" );
           return WEB_BAD_REQUEST;
       case AUTH_FAILURE:
           return WEB_INTERNAL_SERVER_ERROR;
       default:
           return WEB_REPLY_OK;
    }
}

CGI_HEADER_FUNCTION(auth_result_t, Login, char *username, char *password)
{
    auth_result_t auth_result;
    char session_id[CGI_SESSION_COOKIE_SIZE + 1];

    auth_result = AuthStartSession( username, password, session_id );

    if(auth_result == AUTH_SUCCESS) {
      CGI_HEADER_CALL(SetCookie, "sessionId", session_id);
    }

    return auth_result;
}
