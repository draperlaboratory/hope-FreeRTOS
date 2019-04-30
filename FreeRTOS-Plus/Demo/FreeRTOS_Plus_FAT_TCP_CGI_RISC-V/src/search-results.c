/*
 * File: search-results.c
 *
 * Description:
 *   Process the search results form for the Draper medical demo.
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
#include "database.h"
#include "cgi-args.h"
#include "cgi-util.h"
#include "auth.h"
#include "user.h"
#include "medical.h"
#include "FreeRTOS.h"
#include "FreeRTOS_HTTP_commands.h"


database_search_result_t * GetSearchResult( char *cgistr );

CGI_FUNCTION(BaseType_t, ShowSearchResult, database_search_result_t *search_result);

int not_stack_smash = 0;

BaseType_t CgiSearchResults( char *pcWriteBuffer, size_t xWriteBufferLen,
    char *pcHeaderBuffer, size_t xHeaderBufferLen,
    const char *pcCgiArgs )
{
  char session_id[AUTH_SESSION_ID_SIZE] = { 0 };

  user_t *user;
  database_search_result_t *search_result;

  CgiArgValue(session_id, sizeof(session_id), "sessionId", pcCgiArgs);

  user = AuthCheckSessionId(session_id);
  if (user == NULL) {
    return WEB_UNAUTHORIZED;
  }

  search_result = GetSearchResult(pcCgiArgs);
  
  return CGI_IMPL_CALL(ShowSearchResult, search_result);
}

void ParseSearchCgiString(char *pcCgiArgs, char *type, char *first_name,
			 char *last_name, char *address, char *condition) {

  char ltype[2] = { 0 };
  char lfirst_name[USER_NAME_LENGTH] = { 0 };
  char llast_name[USER_NAME_LENGTH] = { 0 };
  char laddress[USER_ADDRESS_LENGTH] = { 0 };
  char lcondition[MEDICAL_NAME_LENGTH] = { 0 };

  CgiArgValue(lfirst_name, USER_NAME_LENGTH, "firstname", pcCgiArgs);
  memcpy(first_name, lfirst_name, sizeof(lfirst_name));

  CgiArgValue(llast_name, USER_NAME_LENGTH, "lastname", pcCgiArgs);
  memcpy(last_name, llast_name, sizeof(llast_name));

  CgiArgValue(laddress, USER_ADDRESS_LENGTH, "address", pcCgiArgs);
  memcpy(address, laddress, sizeof(laddress));

  CgiArgValue(lcondition, MEDICAL_NAME_LENGTH, "condition", pcCgiArgs);
  memcpy(condition, lcondition, sizeof(lcondition));

  printf("Copying \"type\" field to vulnerable buffer\n");
  // bad length argument permits stack-based buffer overflow 
  CgiArgValue(ltype, USER_ADDRESS_LENGTH, "type", pcCgiArgs);

  // only overflow local buffer in current function, not caller
  memcpy(type, ltype, sizeof(ltype));
}

database_search_result_t *
GetSearchResult( char *cgistr )
{
  char type[2] = { 0 };
  char first_name[USER_NAME_LENGTH] = { 0 };
  char last_name[USER_NAME_LENGTH] = { 0 };
  char address[USER_ADDRESS_LENGTH] = { 0 };
  char condition[MEDICAL_NAME_LENGTH] = { 0 };

  database_search_result_t *search_result;
  user_type_t search_type;

  ParseSearchCgiString(cgistr, type, first_name, last_name, address, condition);
  if(not_stack_smash) {
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("addi s0, sp, 336");
    printf("Executing payload...\n");
    DatabaseAddUser(UserCreate("misterhacker", "password", "Mister", "Hacker", "555 Technology Square"));
  }

  search_type = atoi(type);

  search_result = DatabaseSearch(search_type,
				 strlen(first_name) ? first_name : NULL,
				 strlen(last_name)  ? last_name : NULL,
				 strlen(address)    ? address : NULL,
				 strlen(condition)  ? condition : NULL);


  return search_result;
}

CGI_FUNCTION(BaseType_t, ShowSearchResult, database_search_result_t *search_result)
{
  database_search_result_node_t *node;
  char *user_full_name;

  CGI_PRINTF("<p>Retrieved %lu search result(s)</p>\n", search_result->count);

  if(search_result->count == 0) {
    return WEB_REPLY_OK;
  }

  CGI_PRINTF("<table style=\"width:100%%\" align=\"left\" border=\"1\">\n");

  CGI_PRINTF("<tr>\n");
  CGI_PRINTF("<th>Name</th>\n");
  CGI_PRINTF("<th>Address</th>\n");
  CGI_PRINTF("</tr>\n");

  for(node = search_result->head; node != NULL; node = node->next) {
    CGI_PRINTF("<tr>\n");

    user_full_name = UserFullName(node->user);
    if(user_full_name == NULL) {
      return WEB_INTERNAL_SERVER_ERROR;
    }

    CGI_PRINTF("<td>");
    CGI_PRINTF("<a href=\"http://172.25.218.200/user-details.html?username=%s\">", node->user->username);
    CGI_PRINTF("</a>");
    CGI_CALL(HtmlEscape, user_full_name);
    CGI_PRINTF("</td>\n");

    CGI_PRINTF("<td>");
    CGI_CALL(HtmlEscape, node->user->address);
    CGI_PRINTF("</td>\n");
    CGI_PRINTF("</tr>\n");

    free(user_full_name);
  }

  CGI_PRINTF("</table>\n");

  return WEB_REPLY_OK;
}
