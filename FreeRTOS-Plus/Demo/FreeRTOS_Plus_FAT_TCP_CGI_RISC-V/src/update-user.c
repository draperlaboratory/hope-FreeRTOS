/*
 * File: update-user.c
 *
 * Description:
 *   Process the update-user form for the Draper medical demo.
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
#include <stdbool.h>
#include "http_statuses.h"
#include "database.h"
#include "cgi-args.h"
#include "cgi-util.h"
#include "auth.h"
#include "sample.h"
#include "user.h"
#include "FreeRTOS.h"

CGI_FUNCTION(void, UpdateUser, user_t *user, char *address);

BaseType_t CgiUpdateUser( char *pcWriteBuffer, size_t xWriteBufferLen,
    char *pcHeaderBuffer, size_t xHeaderBufferLen,
    const char *pcCgiArgs )
{
  user_t *user;
  char session_id[AUTH_SESSION_ID_SIZE] = { 0 };
  char new_address[USER_ADDRESS_LENGTH + 1] = { 0 };

  CgiArgValue( new_address, sizeof(new_address), "address", pcCgiArgs );
  CgiArgValue( session_id, sizeof(session_id), "sessionId", pcCgiArgs );

  user = AuthCheckSessionId(session_id);
  if (user == NULL) {
    return HTTP_UNAUTHORIZED;
  }

  /* user = UserCreate("user1", "password", "John", "Doe", "123 hello world"); */
  /* if(user == NULL) { */
  /*   return HTTP_INTERNAL_SERVER_ERROR; */
  /* } */
  /* MedicalSetPatient(user); */
  /* DatabaseAddUser(user); */
  /* AuthStartSession("user1", "password", session_id); */
  /*  */
  /* CGI_IMPL_HEADER_CALL(SetCookie, "sessionId", session_id); */

  CGI_IMPL_CALL(UpdateUser, user, new_address);

  return HTTP_OK;
}

CGI_FUNCTION(void, UpdateUser, user_t *user, char *address)
{
  printf("strlen new address: %d\n", strlen(address));
  UserUpdateAddress(user, address);

  // redirect after form processing
  CGI_PRINTF("dashboard.html");
}
