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

auth_result_t Login(char *username, char *password, char *pcHeaderBuffer, size_t xHeaderBufferLen);

BaseType_t CgiLogin( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs )
{
    BaseType_t rv = HTTP_OK;

    /* if( SampleConfiguration() == false ) */
    /* { */
    /*     CGI_PRINTF("CGI script error" ); */
    /*     return HTTP_INTERNAL_SERVER_ERROR; */
    /* } */

    char username[KEY_SIZE_MAX] = { 0 };
    char password[KEY_SIZE_MAX] = { 0 };
    CgiArgValue( username, sizeof(username), "username", pcCgiArgs );
    CgiArgValue( password, sizeof(password), "password", pcCgiArgs );

    auth_result_t login_result = Login( username, password, pcHeaderBuffer, xHeaderBufferLen );

    switch( login_result )
    {
       case AUTH_SUCCESS:
           CGI_PRINTF("dashboard.html" );
           rv =  HTTP_SEE_OTHER;
           break;
       case AUTH_USER_NOT_FOUND:
           CGI_PRINTF("User not found in database" );
           rv =  HTTP_BAD_REQUEST;
           break;
       case AUTH_INVALID_CREDENTIALS:
           CGI_PRINTF("Invalid password" );
           rv =  HTTP_BAD_REQUEST;
           break;
       case AUTH_NOT_LOGGED_IN:
           CGI_PRINTF("One or more empty fields" );
           rv =  HTTP_BAD_REQUEST;
           break;
       default:
           break;
    }

    return rv;
}

auth_result_t Login(char *username, char *password, char *pcHeaderBuffer, size_t xHeaderBufferLen)
{
    auth_result_t auth_result;
    char session_id[CGI_SESSION_COOKIE_SIZE + 1];

    if( (strlen( username ) == 0) || (strlen( password ) == 0) )
    {
        return AUTH_NOT_LOGGED_IN;
    }

    auth_result = AuthStartSession( username, password, session_id );

    switch( auth_result )
    {
       case AUTH_SUCCESS:
           CGI_HEADER_CALL(SimpleCookie, "sessionId", session_id);
           break;
       default:
           break;
    }

    return auth_result;
}
