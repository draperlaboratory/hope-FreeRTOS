/*
 * File: user-details.c
 *
 * Description:
 *   Process the user-details form for the Draper medical demo.
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "database.h"
#include "cgi-args.h"
#include "cgi-util.h"
#include "auth.h"
#include "sample.h"
#include "medical.h"
#include "user.h"
#include "database.h"
#include "FreeRTOS.h"
#include "FreeRTOS_HTTP_commands.h"

#define DETAILS_TYPE_LENGTH 0x10

typedef enum _user_details_type_t {
  USER_DETAILS_BASIC,
  USER_DETAILS_ADVANCED,
  USER_DETAILS_DEBUG,
} user_details_type_t;

CGI_FUNCTION(void, ShowDetails, user_t *user);
CGI_FUNCTION(void, DetailsDebug, user_t *user);
CGI_FUNCTION(void, PatientDetails, patient_t *patient);
CGI_FUNCTION(void, DoctorDetails, doctor_t *doctor);
CGI_FUNCTION(user_t *, GetFormUser, char *username);

user_details_type_t GetDetailsType(char *type);

BaseType_t CgiUserDetails( char *pcWriteBuffer, size_t xWriteBufferLen,
    char *pcHeaderBuffer, size_t xHeaderBufferLen,
    const char *pcCgiArgs )
{
  user_t *user;
  user_t *form_user;
  char session_id[AUTH_SESSION_ID_SIZE] = { 0 };
  char username[USER_NAME_LENGTH] = { 0 };
  char details_type[DETAILS_TYPE_LENGTH] = { 0 };

  CgiArgValue( session_id, sizeof(session_id), "sessionId", pcCgiArgs );
  CgiArgValue( username, sizeof(username), "username", pcCgiArgs );
  CgiArgValue( details_type, sizeof(details_type), "type", pcCgiArgs );

  /* user = AuthCheckSessionId(session_id); */
  /* if (user == NULL) { */
  /*   return WEB_UNAUTHORIZED; */
  /* } */

  user = UserCreate("user1", "password", "John", "Doe", "123 hello world");
  if(user == NULL) {
    return WEB_INTERNAL_SERVER_ERROR;
  }

  form_user = CGI_IMPL_CALL(GetFormUser, username);
  if(form_user == NULL) {
    return WEB_INTERNAL_SERVER_ERROR;
  }
  MedicalSetPatient(form_user);

  CGI_IMPL_CALL(ShowDetails, form_user);

  switch(GetDetailsType(details_type)) {
    case USER_DETAILS_DEBUG:
      CGI_IMPL_CALL(DetailsDebug, form_user);
    case USER_DETAILS_ADVANCED:
      switch(form_user->type) {
        case USER_PATIENT:
          PatientDetails(&pcWriteBuffer, &xWriteBufferLen, MedicalGetPatient(form_user));
          break;
        case USER_DOCTOR:
          DoctorDetails(&pcWriteBuffer, &xWriteBufferLen, MedicalGetDoctor(form_user));
        default:
          break;
      }
    default:
      break;
  }

  return WEB_REPLY_OK;
}

user_details_type_t
GetDetailsType(char *type)
{
  if(strcmp(type, "advanced") == 0) {
    return USER_DETAILS_ADVANCED;
  }
  if(strcmp(type, "debug") == 0) {
    return USER_DETAILS_DEBUG;
  }

  return USER_DETAILS_BASIC;
}

CGI_FUNCTION(user_t *, GetFormUser, char *username)
{
  user_t *user;

  user = DatabaseGetUser(username);

  if(user == NULL) {
    CGI_PRINTF("<p>User not found in database</p>");
    return NULL;
  }

  /* user = UserCreate(username, "password", "John", "Doe", "123 hello world"); */
  /* if(user == NULL) { */
  /*   return NULL; */
  /* } */

  return user;
}

CGI_FUNCTION(void, PatientDetails, patient_t *patient)
{
  size_t i, j;
  char *doctor_name;

  CGI_CALL(Whitespace, 2);

  CGI_PRINTF("<table style=\"width:100%%\" align=\"left\">\n");

  CGI_PRINTF("<tr>\n");
  CGI_PRINTF("<th>Medical Condition</th>\n");
  CGI_PRINTF("<th>Doctor</th>\n");
  CGI_PRINTF("<th>Prescriptions</th>\n");
  CGI_PRINTF("<th>Notes</th>\n");
  CGI_PRINTF("</tr>\n");
  
  for(i = 0; i < patient->record_count; i++) {
    CGI_PRINTF("<tr>\n");

    CGI_PRINTF("<td>");
    CGI_CALL(HtmlEscape, patient->records[i].condition);
    CGI_PRINTF("</td>\n");

    doctor_name = UserFullName(patient->records[i].doctor_user);
    if(doctor_name == NULL) {
      exit(EXIT_FAILURE);
    }
    CGI_PRINTF("<td>");
    CGI_CALL(HtmlEscape, doctor_name);
    CGI_PRINTF("</td>\n");
    free(doctor_name);

    CGI_PRINTF("<td>");
    for(j = 0; j < patient->records[i].treatment_count; j++) {
      CGI_PRINTF("<p>");
      CGI_CALL(HtmlEscape, patient->records[i].treatments[j].name);
      CGI_PRINTF(", %u %s", patient->records[i].treatments[j].dose,
          patient->records[i].treatments[j].unit);
    }
    CGI_PRINTF("</td>\n");

    CGI_PRINTF("<td>");
    CGI_CALL(HtmlEscape, patient->records[i].notes);
    CGI_PRINTF("</td>\n");
    CGI_PRINTF("</tr>\n");
  }

  CGI_PRINTF("</table>\n");
}

CGI_FUNCTION(void, DoctorDetails, doctor_t *doctor)
{
  size_t i;

  CGI_CALL(Whitespace, 2);

  CGI_PRINTF("<table style=\"width:100%%\" align=\"left\">\n");

  CGI_PRINTF("<tr>\n");
  CGI_PRINTF("<th>Certifications</th>\n");
  CGI_PRINTF("<th>Year Received</th>\n");
  CGI_PRINTF("</tr>\n");

  for(i = 0; i < doctor->cert_count; i++) {
    CGI_PRINTF("<tr>\n");
    CGI_PRINTF("<td>");
    CGI_CALL(HtmlEscape, doctor->certs[i].condition);
    CGI_PRINTF("</td>\n");

    CGI_PRINTF("<td>%lu</td>\n", doctor->certs[i].year_received);
    CGI_PRINTF("</tr>\n");
  }

  CGI_PRINTF("</table>\n");
}

CGI_FUNCTION(void, ShowDetails, user_t *user)
{
  char *user_full_name;

	CGI_PRINTF("<p>\n");
	CGI_PRINTF("Showing details for user ");
  CGI_CALL(HtmlEscape, user->first_name);
  CGI_PRINTF(": </p>\n");

  CGI_CALL(FormStart, CGI_FORM_GET, "user-details.html");
  CGI_PRINTF("<input type=\"submit\" value=\"Contact\"></input>\n");
  CGI_CALL(FormEnd);

  CGI_CALL(Whitespace, 1);

  CGI_PRINTF("<table style=\"width:100%%\" align=\"left\" border=\"1\">\n");
  CGI_PRINTF("<tr>\n");
  CGI_PRINTF("<th>Full Name</th>\n");
  CGI_PRINTF("<th>Address</th>\n");
  CGI_PRINTF("</tr>\n");

  user_full_name = UserFullName(user);
  if(user_full_name == NULL) {
    exit(EXIT_FAILURE);
  }

  CGI_PRINTF("<tr>\n");
  CGI_PRINTF("<td>");
  CGI_CALL(HtmlEscape, user_full_name);
  CGI_PRINTF("</td>");
  CGI_PRINTF("<td>");
  CGI_CALL(HtmlEscape, user->address);
  CGI_PRINTF("</td>");
  CGI_PRINTF("</tr>\n");
  CGI_PRINTF("</table>\n");
}

CGI_FUNCTION(void, DetailsDebug, user_t *user)
{
  CGI_PRINTF("<p>username: %s</p>", user->username);
  // XXX: password leakage over network
  CGI_PRINTF("<p>password: %s</p>", user->password);
  CGI_PRINTF("<p>first_name: %s</p>", user->first_name);
  CGI_PRINTF("<p>last_name: %s</p>", user->last_name);
  CGI_PRINTF("<p>address: %s</p>", user->address);
  CGI_CALL(Whitespace, 2);
}
