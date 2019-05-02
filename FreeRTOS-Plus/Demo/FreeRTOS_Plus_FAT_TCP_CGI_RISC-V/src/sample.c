#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sample.h"
#include "database.h"
#include "auth.h"
#include "medical.h"
#include "cgi-util.h"

bool
SampleConfiguration()
{
  user_t *user;
  user_t *doctor_user;
  medical_record_t *record;

  user = UserCreate("patient_user1", "password1", "John", "Doe", "123 Main St");
  DatabaseAddUser(user);
  MedicalSetPatient(user);

  doctor_user = UserCreate("the_doctor", "password2", "Joe", "Schmoe", "456 Second St");
  DatabaseAddUser(doctor_user);
  MedicalSetDoctor(doctor_user);

  if(MedicalAddCert(doctor_user, "Carpal tunnel", 2010) != MEDICAL_SUCCESS) {
    return false;
  }
  if(MedicalAddCert(doctor_user, "Back pain", 2011) != MEDICAL_SUCCESS) {
    return false;
  }

  AuthSetCurrentUserType(doctor_user);
  if(MedicalAddRecord(doctor_user, user,
     "Carpal tunnel", "Take medication twice daily.", &record)
     != MEDICAL_SUCCESS) {
    return false;
  }

  if(MedicalAddTreatment(doctor_user, record, "Ibuprofen", 200, "mg")
     != MEDICAL_SUCCESS) {
    return false;
  }
  AuthClearCurrentUserType();

  return true;
}
