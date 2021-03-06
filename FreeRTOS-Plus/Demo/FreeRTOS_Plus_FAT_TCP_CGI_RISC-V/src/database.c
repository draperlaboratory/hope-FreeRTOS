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

#include "database.h"
#include "hashtable.h"
#include "medical.h"

#define DATABASE_KEY_LENGTH USER_NAME_LENGTH
#define DATABASE_CAPACITY 0x10

static hash_table_t database_table;


static database_search_result_t *
DatabaseSearchResultAlloc(void)
{
  database_search_result_t *result;

  result = malloc(sizeof(database_search_result_t));
  if(result == NULL) {
    return NULL;
  }

  result->head = malloc(sizeof(database_search_result_node_t));
  if(result->head == NULL) {
    free(result);
    return NULL;
  }
  result->head->user = NULL;
  result->head->next = NULL;

  result->count = 0;

  return result;
}

static bool
DatabaseSearchResultAppend(database_search_result_t *result, user_t *user)
{
  database_search_result_node_t *new_node;
  database_search_result_node_t *iter_node;

  if(result->head->user == NULL) {
    result->head->user = user;
    result->count++;
    return true;
  }

  new_node = malloc(sizeof(database_search_result_node_t));
  if(new_node == NULL) {
    return false;
  }

  new_node->user = user;
  new_node->next = NULL;

  iter_node = result->head;
  while(iter_node->next != NULL) {
    iter_node = iter_node->next;
  }

  iter_node->next = new_node;
  result->count++;

  return true;
}

bool 
DatabaseInit(void)
{
  int result;

  result = HashTableSetup(&database_table,
                          DATABASE_KEY_LENGTH,
                          sizeof(user_t *),
                          DATABASE_CAPACITY);

  if(result == HASH_TABLE_ERROR) {
    return false;
  }

  return true;
}

bool
DatabaseAddUser(user_t *user)
{
  if(HashTableContains(&database_table, user->username) == HASH_TABLE_FOUND) {
    return false;
  }

  if(HashTableInsert(&database_table, user->username, &user) == HASH_TABLE_ERROR) {
    return false;
  }

  return true;
}

bool
DatabaseRemoveUser(user_t *user)
{
  if(HashTableErase(&database_table, user->username) == HASH_TABLE_ERROR) {
    return false;
  }

  return true;
}

user_t *
DatabaseGetUser(char *username)
{
  char key[USER_NAME_LENGTH];
  user_t **user;
  user_t *result;

  memset(key, '\0', USER_NAME_LENGTH);
  snprintf(key, USER_NAME_LENGTH, "%s", username);

  user = HashTableLookup(&database_table, key);
  result = *user;

  return result;
}

size_t DatabaseSize()
{
  return database_table.size;
}

database_search_result_t *
DatabaseSearch(user_type_t type, char *first_name, char *last_name, char *address, char *condition)
{
  size_t i;
  size_t size;
  user_t **users;
  database_search_result_t *result;

  size = DatabaseSize();
  users = DatabaseUserList();
  if(users == NULL) {
    return NULL;
  }

  result = DatabaseSearchResultAlloc();
  if(result == NULL) {
    return NULL;
  }
  
  for(i = 0; i < size; i++) {
    if((type != USER_UNKNOWN) && (users[i]->type != type)) {
      continue;
    }

    if((first_name != NULL) &&
       (strcmp(first_name, users[i]->first_name) != 0)) {
      continue;
    }
    if((last_name != NULL) &&
       (strcmp(last_name, users[i]->last_name) != 0)) {
      continue;
    }
    if((address != NULL) &&
       (strcmp(address, users[i]->address) != 0)) {
      continue;
    }

    if((type == USER_DOCTOR) && (condition != NULL)) {
      if(MedicalDoctorCertified(MedicalGetDoctor(users[i]), condition) == false) {
        continue;
      }
    }

    if(DatabaseSearchResultAppend(result, users[i]) == false) {
      free(users);
      DatabaseSearchResultFree(result);
      return NULL;
    }
  }

  free(users);

  return result;
}

user_t **
DatabaseUserList(void)
{
  user_t ***user_ptrs;
  user_t **users;
  size_t i;

  user_ptrs = (user_t ***)HashTableToArray(&database_table);
  if(user_ptrs == NULL) {
    return NULL;
  }

  users = malloc(sizeof(user_t) * database_table.size);
  if(users == NULL) {
    return NULL;
  }

  for(i = 0; i < database_table.size; i++) {
    users[i] = *user_ptrs[i];
  }

  free(user_ptrs);

  return users;
}

void
DatabaseSearchResultFree(database_search_result_t *result)
{
  database_search_result_node_t *node;

  node = result->head;

  while(node != NULL) {
    free(node);
    node = node->next;
  }

  free(result);
}
