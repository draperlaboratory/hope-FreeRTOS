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
#include "database.h"
#include "cgi-args.h"
#include "cgi-util.h"
#include "auth.h"
#include "sample.h"
#include "FreeRTOS_HTTP_commands.h"

#define CGI_RESPONSE_LEN_MAX 2048
#define CGI_HEADER_LEN_MAX 256

CGI_FUNCTION(bool, RegisterNewUser, char *username, char *password, char *first_name, char *last_name, char *address);

BaseType_t CgiRegister( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs )
{
    char username[KEY_SIZE_MAX] = { 0 };
    char password[KEY_SIZE_MAX] = { 0 };
    char first_name[KEY_SIZE_MAX] = { 0 };
    char last_name[KEY_SIZE_MAX] = { 0 };
    char address[KEY_SIZE_MAX] = { 0 };

    CgiArgValue( username, sizeof(username), "username", pcCgiArgs );
    CgiArgValue( password, sizeof(password), "password", pcCgiArgs );
    CgiArgValue( first_name, sizeof(first_name), "first_name", pcCgiArgs );
    CgiArgValue( last_name, sizeof(last_name), "last_name", pcCgiArgs );
    CgiArgValue( address, sizeof(address), "address", pcCgiArgs );
    
    if(CGI_IMPL_CALL(RegisterNewUser, username, password, first_name, last_name, address) == false) {
      return WEB_INTERNAL_SERVER_ERROR;
    }

    return WEB_REPLY_OK;
}

CGI_FUNCTION(bool, RegisterNewUser, char *username, char *password, char *first_name, char *last_name, char *address)
{
    user_t *user;

    user = UserCreate(username, password, first_name, last_name, address);   
    if(user == NULL) {
      CGI_PRINTF("Failed to create user");
      return false;
    }

    if(DatabaseAddUser(user) == false) {
      CGI_PRINTF("Failed to add user to database");
      return false;
    }

    // redirect after form processing
    CGI_PRINTF("login.html");
    return true;
}
