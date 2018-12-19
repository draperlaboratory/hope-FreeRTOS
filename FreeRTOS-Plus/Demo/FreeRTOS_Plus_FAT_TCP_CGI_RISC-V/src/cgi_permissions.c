/*
 * File: cgi_permissions.c
 *
 * Description:
 *   This file implements the Permissions, Privilege, and Access Control CGI.
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

/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* Defines */
#define MESSAGE_LEN 256

/* Function definitions */
BaseType_t CgiPermissions( char *pcWriteBuffer, size_t xWriteBufferLen,
                                  const char *pcCgiArgs )
{
    (void)pcCgiArgs;

    /*
     * This is adminstrator-only data.
     *
     * To protect from violating permissions, tag this variable as "classified".
     */
    char adminData[] = "<b>ADMIN DATA:</b> Lorem ipsum dolor sit amet, consectetur adipiscing elit.";

    /*
     * Tag this variable as "unclassified".
     */
    char message[MESSAGE_LEN] = { 0 };

    /*
     * This line will violate the policy--writing from "classified" memory to "unclassified" memory
     * is not allowed.
     */
    strncpy( message, adminData, MESSAGE_LEN );

    strncpy( pcWriteBuffer, message, xWriteBufferLen );

    return 0;
}

