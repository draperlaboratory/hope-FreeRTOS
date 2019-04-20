#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user.h"

#define USER_FULL_NAME_LENGTH (USER_NAME_LENGTH * 2 + 3)

int isp_tag_password_char = 0xa;

void __attribute__ ((noinline)) 
UserTagPass(user_t *user) {

  int i;
  for ( i = 0; i < USER_PASSWORD_LENGTH/(sizeof(int)); i+=4 )
    *(int*)(&(user->password[i])) = isp_tag_password_char;

  return;
}

user_t *
UserCreate(char *username, char *password, char *first_name, char *last_name,
           char *address)
{
  user_t *user;

  user = malloc(sizeof(user_t));
  if(user == NULL) {
    return NULL;
  }
  memset(user, 0, sizeof(user_t));

  UserTagPass(user);
  
  user->type = USER_UNKNOWN;
  user->data = NULL;

  snprintf(user->username, sizeof(user->username) + 1, "%s", username);
  snprintf(user->password, sizeof(user->password) + 1, "%s", password);
  snprintf(user->first_name, sizeof(user->first_name) + 1, "%s", first_name);
  snprintf(user->last_name, sizeof(user->last_name) + 1, "%s", last_name); 
  snprintf(user->address, sizeof(user->address) + 1, "%s", address);
  
  return user;
}

void
UserDestroy(user_t *user)
{
  if(user == NULL) {
    return;
  }

  if(user->data != NULL) {
    free(user->data);
  }

  free(user);
}

volatile user_type_t _temp_user_type_assigner;

void __attribute__((noinline))
UserSetType(user_t *user, user_type_t type)
{
  _temp_user_type_assigner = type;
  user->type = _temp_user_type_assigner;
}

char *
UserFullName(user_t *user)
{
  char *result;
  char buffer[USER_FULL_NAME_LENGTH];

  snprintf(buffer, sizeof(buffer), "%s, %s", user->last_name, user->first_name);
  result = strdup(buffer);

  return result;
}

void
UserUpdateAddress(user_t *user, char *address)
{
  snprintf(user->address, sizeof(user->address) + 1, "%s", address);
}
