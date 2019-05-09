#include <stdlib.h>
#include "user.h"
#include "medical.h"
#include "auth.h"

void NegativeTestPassword(void)
{
  user_t *user;

  user = UserCreate("user", "password123", "User", "One", "123 Main St.");
  if(user == NULL) {
    printf("TEST ERROR: Failed to create user\n");
    return;
  }

  printf("About to leak password. Should violate password policy\n");
  printf("User password: %s\n", user->password);
  printf("WARNING: NO VIOLATION. OPERATION SUCCEEDED\n");
}

void NegativeTestUserType(void)
{
  user_t *user;

  user = UserCreate("user", "toor", "User", "Two", "123 Main St.");
  MedicalSetPatient(user);
  if(user == NULL) {
    printf("TEST ERROR: Failed to create user\n");
  }

  printf("About to try to set user to doctor. Should violate userType policy\n");
  MedicalSetDoctor(user);
  printf("WARNING: NO VIOLATION. OPERATION SUCCEEDED. \n");
}

void NegativeTestUserTypeOverwrite(void)
{
  user_t *user;
  char payload[0x80]; 

  user = UserCreate("user", "toor", "User", "Two", "123 Main St.");
  MedicalSetPatient(user);
  if(user == NULL) {
    printf("TEST ERROR: Failed to create user\n");
  }

  printf("About to try to overwrite user type. Should violate userType policy\n");
  memset(payload, 'a', sizeof(payload));
  UserUpdateAddress(user, payload);
  printf("WARNING: NO VIOLATION. OPERATION SUCCEEDED. \n");
}

void NegativeTestPPACDoctor(void)
{
  user_t *patient_user;
  user_t *doctor_user;

  patient_user = UserCreate("patient_user", "password123", "Pat", "Ient", "123 Main St.");
  if(patient_user == NULL) {
    printf("TEST ERROR: Failed to create patient\n");
    return;
  }                                                                         

  doctor_user = UserCreate("doctor_user", "password123", "Pat", "Ient", "123 Main St.");
  if(doctor_user == NULL) {
    printf("TEST_ERROR: Failed to create doctor\n");
    return;
  }

  MedicalSetPatient(patient_user);
  MedicalSetDoctor(doctor_user);

  AuthSetCurrentUserType(patient_user);
  
  printf("About to try to add record as a patient. Should violate ppac policy\n");
  MedicalAddRecord(doctor_user, patient_user, "Fractured Authentication", "The doctor is not logged in", NULL);
  printf("WARNING: NO VIOLATION. OPERATION SUCCEEDED.\n");
}

void NegativeTestPPACPatient(void)
{
  user_t *patient_user1;
  user_t *patient_user2;
  user_t *doctor_user;
  patient_t *patient_data;
  
  patient_user1 = UserCreate("patient_user1", "password123", "Pat", "Ient", "123 Main St.");
  if(patient_user1 == NULL) {
    printf("TEST ERROR: Failed to create patient 1\n");
    return;
  }

  patient_user2 = UserCreate("patient_user2", "password123", "Pat", "Ient", "123 Main St.");
  if(patient_user2 == NULL) {
    printf("TEST ERROR: Failed to create patient 2\n");
    return;
  }

  doctor_user = UserCreate("doctor_user", "password123", "Pat", "Ient", "123 Main St.");
  if(doctor_user == NULL) {
    printf("TEST ERROR: Failed to create doctor\n");
    return;
  }

  MedicalSetPatient(patient_user1);
  MedicalSetPatient(patient_user2);

  MedicalSetDoctor(doctor_user);

  AuthSetCurrentUserType(doctor_user);

  MedicalAddRecord(doctor_user, patient_user1, "Sample Condition", "sample notes", NULL);

  AuthSetCurrentUserType(patient_user2);
   
  printf("About to access non-active patient data. Should violate ppac policy\n");
  patient_data = MedicalGetPatient(patient_user1);
  printf("Patient User 1 has %lu records\n", patient_data->record_count);

  printf("WARNING: NO VIOLATION. OPERATION SUCCEEDED.\n");
}


void NegativeTestStack(void)
{
  char *cgi_stack_payload = ",type:abcdefghij\x44\x46\x40\x20,sessionId:123,firstname:John,lastname:Doe,address:123 hello world,condition:Carpal tunnel,";

  DatabaseInit();

  printf("About to overflow stack buffer in vulnerable CGI function\n");
  GetSearchResult(cgi_stack_payload);
  printf("WARNING: NO VIOLATION. OPERATION SUCCEEDED.\n");

  return;
}
