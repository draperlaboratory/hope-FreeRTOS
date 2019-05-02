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
