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
