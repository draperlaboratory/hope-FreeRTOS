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

static char type[2] = { 0 };
static char first_name[USER_NAME_LENGTH] = { 0 };
static char last_name[USER_NAME_LENGTH] = { 0 };
static char address[USER_ADDRESS_LENGTH] = { 0 };
static char condition[MEDICAL_NAME_LENGTH] = { 0 };

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

void ParseSearchCgiString(char *pcCgiArgs) {

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
  database_search_result_t *search_result;
  user_type_t search_type;

  ParseSearchCgiString(cgistr);
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
    __asm("addi s0, sp, 96");
    printf("Executing payload...\n");
    DatabaseAddUser(UserCreate("misterhacker", "password", "Mister", "Hacker", "555 Technology Square"));
    search_result = malloc(sizeof(database_search_result_t));
    search_result->count = 0;
    return search_result;
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
    CGI_PRINTF("<a href=\"%s/user-details.html?username=%s\">", CGI_BASE_URL, node->user->username);
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
