#ifndef NEGATIVE_TESTS_H
#define NEGATIVE_TESTS_H

void user_type_test(void);
void password_negative_test(void);
void patient_info_leak(void);
void unauth_doctor_routine(void);

// #define negative_test user_type_test
#define negative_test user_type_overwrite_test
// #define negative_test password_negative_test
// #define negative_test patient_info_leak
// #define negative_test unauth_doctor_routine
// #define negative_test stack_smash

#endif
