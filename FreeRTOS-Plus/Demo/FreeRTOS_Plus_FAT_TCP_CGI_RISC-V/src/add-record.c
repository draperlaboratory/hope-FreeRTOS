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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "database.h"
#include "cgi-args.h"
#include "cgi-util.h"
#include "auth.h"
#include "user.h"
#include "medical.h"
#include "FreeRTOS.h"
#include "FreeRTOS_HTTP_commands.h"

CGI_FUNCTION(BaseType_t, UpdateRecord, char *doctor_name, char *patient_name,
    char *condition, char *notes, char *treatment, 
    char *treatment_unit, size_t dose);

BaseType_t CgiAddRecord( char *pcWriteBuffer, size_t xWriteBufferLen,
    char *pcHeaderBuffer, size_t xHeaderBufferLen,
    const char *pcCgiArgs )
{
  user_t *doctor_user;
  user_t *patient_user;
  char session_id[AUTH_SESSION_ID_SIZE] = { 0 };
  char doctor_name[USER_NAME_LENGTH + 1] = { 0 };
  char patient_name[USER_NAME_LENGTH + 1] = { 0 };
  char condition[MEDICAL_NAME_LENGTH + 1] = { 0 };
  char notes[0x100] = { 0 };
  char treatment[MEDICAL_NAME_LENGTH + 1] = { 0 };
  char treatment_unit[MEDICAL_UNIT_NAME_LENGTH + 1] = { 0 };
  char dose_string[0x10] = { 0 };
  size_t dose;
  
  CgiArgValue( session_id, sizeof(session_id), "sessionId", pcCgiArgs );
  CgiArgValue( patient_name, sizeof(patient_name), "patient", pcCgiArgs );
  CgiArgValue( doctor_name, sizeof(doctor_name), "doctor", pcCgiArgs );
  CgiArgValue( condition, sizeof(condition), "condition", pcCgiArgs );
  CgiArgValue( notes, sizeof(notes), "notes", pcCgiArgs );
  CgiArgValue( treatment, sizeof(treatment), "treatment", pcCgiArgs );
  CgiArgValue( treatment_unit, sizeof(treatment_unit), "unit", pcCgiArgs );
  CgiArgValue( dose_string, sizeof(dose_string), "dose", pcCgiArgs );

  dose = atoi(dose_string);

  doctor_user = AuthCheckSessionId(session_id);
  if (doctor_user == NULL) {
    return WEB_UNAUTHORIZED;
  }
  // missing check for doctor user type

  return CGI_IMPL_CALL(UpdateRecord, doctor_name, patient_name, condition, notes, treatment, treatment_unit, dose);
}

CGI_FUNCTION(BaseType_t, UpdateRecord, char *doctor_name, char *patient_name,
    char *condition, char *notes, char *treatment, 
    char *treatment_unit, size_t dose)
{
  user_t *doctor_user;
  user_t *patient_user;
  medical_result_t result;
  medical_record_t *record;

  doctor_user = DatabaseGetUser(doctor_name);
  if(doctor_user == NULL) {
    CGI_PRINTF("User %s not found\n", doctor_name);
    return WEB_BAD_REQUEST;
  }

  patient_user = DatabaseGetUser(patient_name);
  if(patient_user == NULL) {
    CGI_PRINTF("User %s not found\n", patient_name);
    return WEB_BAD_REQUEST;
  }

  result = MedicalAddRecord(doctor_user, patient_user, condition, notes, &record);
  switch(result) {
    case MEDICAL_SUCCESS:
      break;
    case MEDICAL_INVALID_USER:
    case MEDICAL_NOT_CERTIFIED:
      CGI_PRINTF("Bad request, invalid user\n");
      return WEB_BAD_REQUEST;
    default:
      CGI_PRINTF("Internal server error\n");
      return WEB_INTERNAL_SERVER_ERROR;
  }
  
  result = MedicalAddTreatment(doctor_user, record, treatment, dose, treatment_unit);
  switch(result) {
    case MEDICAL_SUCCESS:
      break;
    case MEDICAL_INVALID_USER:
    case MEDICAL_NOT_CERTIFIED:
      CGI_PRINTF("Bad request, invalid user\n");
      return WEB_BAD_REQUEST;
    default:
      CGI_PRINTF("Internal server error\n");
      return WEB_INTERNAL_SERVER_ERROR;
  }

  // redirect after form processing
  CGI_PRINTF("dashboard.html");
  return WEB_REPLY_OK;
}
