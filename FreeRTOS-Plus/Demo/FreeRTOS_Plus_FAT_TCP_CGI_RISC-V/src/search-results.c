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
#include <stdbool.h>
#include <stdlib.h>
#include "http_statuses.h"
#include "database.h"
#include "cgi-args.h"
#include "cgi-util.h"
#include "auth.h"
#include "user.h"
#include "medical.h"
#include "FreeRTOS.h"

database_search_result_t *GetSearchResult(user_t *user,
    char *_first_name, char *_last_name, char *_address,
    char *_condition, char *type_string);
CGI_FUNCTION(void, ShowSearchResult, database_search_result_t *search_result);

BaseType_t CgiSearchResults( char *pcWriteBuffer, size_t xWriteBufferLen,
    char *pcHeaderBuffer, size_t xHeaderBufferLen,
    const char *pcCgiArgs )
{
  char type[2];
  char first_name[USER_NAME_LENGTH];
  char last_name[USER_NAME_LENGTH];
  char address[USER_ADDRESS_LENGTH];
  char condition[MEDICAL_NAME_LENGTH];
  char session_id[KEY_SIZE_MAX] = { 0 };
  user_t *user;
  database_search_result_t *search_result;

  CgiArgValue(session_id, sizeof(session_id), "sessionId", pcCgiArgs);

  if(DatabaseInit() == false) {
    return HTTP_INTERNAL_SERVER_ERROR;
  }

  /* user = AuthCheckSessionId(session_id); */
  /* if (user == NULL) { */
  /*   return HTTP_UNAUTHORIZED; */
  /* } */

  user = UserCreate("user1", "password", "John", "Doe", "123 hello world");
  if(user == NULL) {
    return HTTP_INTERNAL_SERVER_ERROR;
  }
  MedicalSetPatient(user);

  CgiArgValue(first_name, USER_NAME_LENGTH, "firstname", pcCgiArgs);
  CgiArgValue(last_name, USER_NAME_LENGTH, "lastname", pcCgiArgs);
  CgiArgValue(address, USER_ADDRESS_LENGTH, "address", pcCgiArgs);
  CgiArgValue(condition, MEDICAL_NAME_LENGTH, "condition", pcCgiArgs);

  // bad length argument permits stack-based buffer overflow 
  CgiArgValue(type, USER_ADDRESS_LENGTH, "type", pcCgiArgs);

  search_result = GetSearchResult(user, first_name, last_name, address, condition, type);
  CGI_IMPL_CALL(ShowSearchResult, search_result);

  return HTTP_OK;
}

database_search_result_t *
GetSearchResult(user_t *user, char *_first_name, char *_last_name,
    char *_address, char *_condition, char *type_string)
{
  char *first_name = _first_name;
  char *last_name = _last_name;
  char *address = _address;
  char *condition = _condition;
  database_search_result_t *search_result;
  user_type_t search_type;

  if(strlen(_first_name) == 0) {
    first_name = NULL;
  }

  if(strlen(_last_name) == 0) {
    last_name = NULL;
  }

  if(strlen(_address) == 0) {
    address = NULL;
  }

  if(strlen(_condition) == 0) {
    condition = NULL;
  }

  search_type = atoi(type_string);

  switch(user->type) {
    case USER_PATIENT:
      search_result = DatabaseSearch(USER_DOCTOR, first_name, last_name, address, condition);
      break;
    case USER_DOCTOR:
      search_result = DatabaseSearch(search_type, first_name, last_name, address, NULL);
      break;
    default:
      break;
  }

  return search_result;
}

CGI_FUNCTION(void, ShowSearchResult, database_search_result_t *search_result)
{
  database_search_result_node_t *node;
  char *user_full_name;

  CGI_PRINTF("<p>Retrieved %lu search result(s)</p>\n", search_result->count);

  if(search_result->count == 0) {
    return;
  }

  CGI_PRINTF("<table style=\"width:100%%\" align=\"left\">\n");

  CGI_PRINTF("<tr>\n");
  CGI_PRINTF("<th>Name</th>\n");
  CGI_PRINTF("<th>Address</th>\n");
  CGI_PRINTF("</tr>\n");

  for(node = search_result->head; node != NULL; node = node->next) {
    CGI_PRINTF("<tr>\n");

    user_full_name = UserFullName(node->user);
    if(user_full_name == NULL) {
      exit(EXIT_FAILURE);
    }

    CGI_PRINTF("<td>");
    CGI_PRINTF("<a href=\"user-details.cgi?username=%s&type=advanced\">", node->user->username);
    CGI_CALL(HtmlEscape, user_full_name);
    CGI_PRINTF("</a>");
    CGI_PRINTF("</td>\n");

    CGI_PRINTF("<td>");
    CGI_CALL(HtmlEscape, node->user->address);
    CGI_PRINTF("</td>\n");
    CGI_PRINTF("</tr>\n");

    free(user_full_name);
  }

  CGI_PRINTF("</table>\n");
}
