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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user.h"

#define USER_FULL_NAME_LENGTH (USER_NAME_LENGTH * 2 + 3)

uintptr_t isp_tag_password_char = 0xa;

static void __attribute__ ((noinline)) 
UserTagPassword(user_t *user)
{
  size_t i;

  for (i = 0; i < USER_PASSWORD_LENGTH/(sizeof(uintptr_t)); i+=sizeof(uintptr_t)) {
    *(uintptr_t*)(&(user->password[i])) = isp_tag_password_char;
  }

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
  
  user->password[0] = 'h';
  UserTagPassword(user);

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

user_type_t _temp_user_type_assigner;

void __attribute__ ((noinline))
UserSetType(user_t *user, user_type_t type)
{
  _temp_user_type_assigner = type;
  user->type = _temp_user_type_assigner;
}

char *
UserFullName(user_t *user)
{
  char *result;

  result = malloc(USER_FULL_NAME_LENGTH + 1);
  if(result == NULL) {
    return NULL;
  }

  snprintf(result, USER_FULL_NAME_LENGTH, "%s, %s", user->last_name, user->first_name);

  return result;
}

void
UserUpdateAddress(user_t *user, char *address)
{
  snprintf(user->address, sizeof(user->address) + 1, "%s", address);
}

void __attribute__ ((noinline))
UserSetAdmin(user_t *user)
{
  UserSetType(user, USER_ADMINISTRATOR);
}
