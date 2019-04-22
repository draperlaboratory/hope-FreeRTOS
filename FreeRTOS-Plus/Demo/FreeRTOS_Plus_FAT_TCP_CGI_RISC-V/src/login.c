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
#include "http_statuses.h"
#include "FreeRTOS.h"
#include "cgi-args.h"
#include "cgi-util.h"
#include "auth.h"
#include "sample.h"

CGI_HEADER_FUNCTION(auth_result_t, Login, char *username, char *password);
CGI_FUNCTION(BaseType_t, CheckLoginResult, auth_result_t login_result);

BaseType_t CgiLogin( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs )
{
    BaseType_t rv = HTTP_OK;
    char username[KEY_SIZE_MAX] = { 0 }; 
    char password[KEY_SIZE_MAX] = { 0 };
    auth_result_t login_result;

    /* if( SampleConfiguration() == false ) { */
    /*   return HTTP_INTERNAL_SERVER_ERROR; */
    /* } */

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
           return HTTP_OK;
       case AUTH_USER_NOT_FOUND:
           CGI_PRINTF("User not found in database" );
           return HTTP_BAD_REQUEST;
       case AUTH_INVALID_CREDENTIALS:
           CGI_PRINTF("Invalid password" );
           return HTTP_BAD_REQUEST;
       case AUTH_NOT_LOGGED_IN:
           CGI_PRINTF("One or more empty fields" );
           return HTTP_BAD_REQUEST;
       case AUTH_FAILURE:
           return HTTP_SEE_OTHER;
       default:
           return HTTP_OK;
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
