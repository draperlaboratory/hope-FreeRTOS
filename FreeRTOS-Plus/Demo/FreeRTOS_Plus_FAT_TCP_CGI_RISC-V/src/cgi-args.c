/*
 * File: cgiArgs.c
 *
 * Description:
 *   Implement CGI args handling backend.
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

#include <string.h>
#include <stdio.h>

#include "cgi-args.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* Public */
bool CgiArgPresent(const char *argName, const char *cgiStr) {
    bool rv = false;

    if (NULL != cgiStr) {
        char argSeparator[KEY_SIZE_MAX] = { 0 };
        snprintf(",%s:", KEY_SIZE_MAX, argSeparator, argName);
        /* NOTE: assumes fields are pre-sanitized! */
        rv = (NULL != strstr(cgiStr, argSeparator));
    }

    return rv;
}

void CgiArgValue(char *argValue, size_t valueLen, const char *argName, const char *cgiStr) {
    bool found = false;
    if (NULL != cgiStr) {
        char argSeparator[KEY_SIZE_MAX] = { 0 };
        int keyLen = snprintf(argSeparator, KEY_SIZE_MAX, ",%s:", argName);
        /* NOTE: assumes fields are pre-sanitized! */
        char *key = strstr(cgiStr, argSeparator);
        if (NULL != key) {
            char *commaLoc = strstr(&cgiStr[keyLen], ",");
            if (NULL != commaLoc) {
                int actualValueLen = commaLoc - &cgiStr[keyLen];
                strncpy(argValue, &cgiStr[keyLen], MIN(valueLen, actualValueLen));
                found = true;
            }
        }
    }

    if (!found) {
        strncpy(argValue, "", valueLen);
    }
}


/* Private */

