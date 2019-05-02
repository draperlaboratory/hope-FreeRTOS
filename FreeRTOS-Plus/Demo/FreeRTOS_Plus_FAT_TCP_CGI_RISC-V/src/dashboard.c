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
#include <stdlib.h>
#include "user.h"
#include "medical.h"
#include "cgi-args.h"
#include "cgi-util.h"
#include "auth.h"
#include "sample.h"
#include "FreeRTOS.h"
#include "FreeRTOS_HTTP_commands.h"

CGI_FUNCTION(void, CommonDashboard, user_t *user);
CGI_FUNCTION(void, PatientDashboard, patient_t *patient);
CGI_FUNCTION(void, DoctorDashboard, doctor_t *doctor, char *doctor_name);
CGI_FUNCTION(void, AdminDashboard);

BaseType_t CgiDashboard( char *pcWriteBuffer, size_t xWriteBufferLen,
    char *pcHeaderBuffer, size_t xHeaderBufferLen,
    const char *pcCgiArgs )
{
  user_t *user;
  char session_id[AUTH_SESSION_ID_SIZE];

  CgiArgValue( session_id, sizeof(session_id), "sessionId", pcCgiArgs );

  user = AuthCheckSessionId(session_id);
  if (user == NULL) {
    return WEB_UNAUTHORIZED;
  }

  CGI_IMPL_CALL(CommonDashboard, user);

  return WEB_REPLY_OK;
}

static char *UserTypeString(user_t *user)
{
  switch(user->type) {
    case USER_PATIENT:
      return "Patient";
    case USER_DOCTOR:
      return "Doctor";
    case USER_ADMINISTRATOR:
      return "Administrator";
  }

  return "Unknown";
}

CGI_FUNCTION(void, CommonDashboard, user_t *user)
{
  char *user_full_name;

  CGI_PRINTF("<p>Welcome, ");

  CGI_CALL(HtmlEscape, user->first_name);
  CGI_PRINTF("!</p>\n");
  CGI_CALL(Whitespace, 1);

  CGI_PRINTF("<p>User type: %s</p>\n", UserTypeString(user));

  CGI_CALL(Whitespace, 1);

  CGI_CALL(LinkButton, "Find a Doctor", "search.html");
  CGI_CALL(LinkButton, "Logout", "logout.html");
  CGI_CALL(LinkButton, "Update Info", "update-user.html");
  CGI_CALL(Whitespace, 1);

  CGI_PRINTF("<table style=\"width:100%%\" align=\"left\">\n");
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

  switch(user->type) {
    case USER_PATIENT:
      CGI_CALL(PatientDashboard, MedicalGetPatient(user));
      break;
    case USER_DOCTOR:
      CGI_CALL(DoctorDashboard, MedicalGetDoctor(user), user->username);
      break;
    case USER_ADMINISTRATOR:
      CGI_CALL(AdminDashboard);
      break;
    default:
      break;
  }
}

CGI_FUNCTION(void, PatientDashboard, patient_t *patient)
{
  size_t i, j;
  char *doctor_name;
  medical_record_t *record;

  CGI_CALL(Whitespace, 2);

  CGI_PRINTF("<table style=\"width:100%%\" align=\"left\">\n");

  CGI_PRINTF("<tr>\n");
  CGI_PRINTF("<th>Medical Condition</th>\n");
  CGI_PRINTF("<th>Doctor</th>\n");
  CGI_PRINTF("<th>Prescriptions</th>\n");
  CGI_PRINTF("<th>Notes</th>\n");
  CGI_PRINTF("</tr>\n");

  for(i = 0; i < patient->record_count; i++) {
    record = &patient->records[i];
    CGI_PRINTF("<tr>\n");

    CGI_PRINTF("<td>");

    CGI_CALL(HtmlEscape, record->condition);
    CGI_PRINTF("</td>\n");

    doctor_name = UserFullName(record->doctor_user);
    if(doctor_name == NULL) {
      exit(EXIT_FAILURE);
    }
    CGI_PRINTF("<td>");
    CGI_PRINTF("<a href=\"http://172.25.218.200/user-details.html?username=%s\">", record->doctor_user->username);
    CGI_CALL(HtmlEscape, doctor_name);
    CGI_PRINTF("</a>");
    CGI_PRINTF("</td>\n");
    free(doctor_name);

    CGI_PRINTF("<td>");

    for(j = 0; j < record->treatment_count; j++) {
      CGI_PRINTF("<p>");
      CGI_CALL(HtmlEscape, record->treatments[j].name);
      CGI_PRINTF(", %u %s", record->treatments[j].dose,
          record->treatments[j].unit);
      CGI_PRINTF("</p>");
    }

    CGI_PRINTF("</td>\n");

    CGI_PRINTF("<td>");

    CGI_CALL(HtmlEscape, record->notes);
    CGI_PRINTF("</td>\n");
    CGI_PRINTF("</tr>\n");
  }

  CGI_PRINTF("</table>\n");
}

CGI_FUNCTION(void, DoctorDashboard, doctor_t *doctor, char *doctor_name)
{
  size_t i;
  char *user_full_name;

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

  CGI_CALL(Whitespace, 2);

  CGI_PRINTF("<p>Patient count: %lu\n</p>", doctor->patient_count);
  CGI_PRINTF("<table style=\"width:100%%\" align=\"left\">\n");

  CGI_PRINTF("<tr>\n");
  CGI_PRINTF("<th>Patient Name</th>\n");
  CGI_PRINTF("<th>Patient Address</th>\n");
  CGI_PRINTF("</tr>\n");

  for(i = 0; i < doctor->patient_count; i++) {
    user_full_name = UserFullName(doctor->patient_users[i]);
    if(user_full_name == NULL) {
      exit(EXIT_FAILURE);
    }

    CGI_PRINTF("<tr>\n");

    CGI_PRINTF("<td>");
    CGI_CALL(HtmlEscape, user_full_name);
    CGI_PRINTF("</td>");

    CGI_PRINTF("<td>");
    CGI_CALL(HtmlEscape, doctor->patient_users[i]->address);
    CGI_PRINTF("</td>");

    CGI_PRINTF("<td>");
    CGI_PRINTF("<a href=\"http://172.25.218.200/add-record.html?patient=%s&amp;doctor=%s\"><button>Add Record</button></a>", doctor->patient_users[i]->username, doctor_name);
    CGI_PRINTF("</td>");

    CGI_PRINTF("</tr>\n");
    free(user_full_name);
  }

  CGI_PRINTF("</table>\n");
}

CGI_FUNCTION(void, AdminDashboard)
{
  CGI_CALL(Whitespace, 2);
  /* CGI_PRINTF("<p>Congrats, you're an admin now!</p>\n"); */
}
