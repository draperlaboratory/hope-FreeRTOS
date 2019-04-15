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
#include "database.h"
#include "FreeRTOS.h"
#include "cgi-args.h"
#include "cgi-util.h"
#include "auth.h"
#include "sample.h"

#define CGI_RESPONSE_LEN_MAX 2048
#define CGI_HEADER_LEN_MAX 256

BaseType_t CgiRegister( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs )
{
    char username[KEY_SIZE_MAX] = { 0 };
    char password[KEY_SIZE_MAX] = { 0 };
    char first_name[KEY_SIZE_MAX] = { 0 };
    char last_name[KEY_SIZE_MAX] = { 0 };
    char address[KEY_SIZE_MAX] = { 0 };
    user_t *user;

    CgiArgValue( username, sizeof(username), "username", pcCgiArgs );
    CgiArgValue( password, sizeof(password), "password", pcCgiArgs );
    CgiArgValue( first_name, sizeof(first_name), "first_name", pcCgiArgs );
    CgiArgValue( last_name, sizeof(last_name), "last_name", pcCgiArgs );
    CgiArgValue( address, sizeof(address), "address", pcCgiArgs );

    user = UserCreate(username, password, first_name, last_name, address);   
    if(user == NULL) {
      return HTTP_INTERNAL_SERVER_ERROR;
    }

    if(DatabaseAddUser(user) == false) {
      return HTTP_INTERNAL_SERVER_ERROR;
    }

    snprintf(pcWriteBuffer, xWriteBufferLen, "login.html");

    return HTTP_OK;
}
