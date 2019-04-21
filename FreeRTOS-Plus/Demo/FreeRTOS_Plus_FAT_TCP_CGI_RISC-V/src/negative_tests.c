#include <stdlib.h>
#include "user.h"
#include "medical.h"
#include "auth.h"

void password_negative_test(void)
{
  user_t *user;

  user = UserCreate("user", "password123", "User", "One", "123 Main St.");
  if(user == NULL) {
    printf("TEST ERROR: Failed to create user\n");
    return;
  }

  // leak will fail with password policy
  printf("ABOUT TO LEAK PASSWORD. SHOULD VIOLATE PASSWORD POLICY\n");
  printf("User password: %s\n", user->password);
  printf("WARNING: NO VIOLATION. OPERATION SUCCEEDED\n");
}

void user_type_test(void)
{
  user_t *user;

  printf("user type test!\n");
  printf("  about to create user \n");
  user = UserCreate("user", "toor", "User", "Two", "123 Main St.");
  printf("  user created!\n");
  printf("    addr = 0x%x\n", user);
  printf("    addr @ type = 0x%x\n", &user->type);
  printf("  about to set type\n");
  MedicalSetPatient(user);
  printf("  type set!\n");
  if(user == NULL) {
    printf("TEST ERROR: Failed to create user\n");
  }

  // illegal attempt tp escalate privilege
  printf("ABOUT TO TRY TO SET USER TO DOCTOR. SHOULD VIOLATE USER TYPE POLICY\n");
  MedicalSetDoctor(user);
  printf("WARNING: NO VIOLATION. OPERATION SUCCEEDED. \n");
}

void unauth_doctor_routine(void)
{
  user_t *patient_user;
  user_t *doctor_user;
  medical_record_t *record;
  
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

  AuthSetCurrentUserType(patient_user->type);
  
  // should fail with PPAC policy since Doctor is not the active user
  printf("ABOUT TO TRY TO ADD RECORD AS A PATIENT. SHOULD VIOLATE PPAC\n");
  MedicalAddRecord(doctor_user, patient_user, "Fractured Authentication", "The doctor is not logged in", &record);
  printf("WARNING: NO VIOLATION. OPERATION SUCCEEDED.\n");
}

void patient_info_leak(void)
{
  user_t *patient_user1;
  user_t *patient_user2;
  user_t *doctor_user;
  patient_t *patient_data;
  medical_record_t *record;
  
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

  AuthSetCurrentUserType(doctor_user->type);
  MedicalAddRecord(doctor_user, patient_user1, "Sample Condition", "sample notes", &record);

  AuthSetCurrentUserType(patient_user2->type);
   
  printf("ABOUT TO ACCESS NON-ACTIVE PATIENT DATA. SHOULD VIOLATE PPAC POLICY\n");

  // should fail with PPAC policy since patient_user1 is not the active user
  patient_data = MedicalGetPatient(patient_user1);
  printf("Patient User 1 has %lu records\n", patient_data->record_count);

  printf("WARNING: NO VIOLATION. OPERATION SUCCEEDED.\n");
}
