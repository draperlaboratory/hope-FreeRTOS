/*
 * File: cgi_leakage.c
 *
 * Description:
 *   This file implements Information Leakage CGI.
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
#include "task.h"

/* App includes. */
//#include "Zynq/x_emacpsif.h"

/* Defines */
#define BUFFER_SIZE (1024)

/* Function declarations */
static void exfilToUart( char *message );

/* Function definitions */
BaseType_t CgiLeakage( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCgiArgs )
{
    (void)pcCgiArgs;

    /*
     * To protect from data exfiltration, tag this memory as "secret".
     *
     * vTaskList() gathers and formats information on the FreeRTOS tasks. It is both a truly
     * sensitive piece of information and proof that the demo is performing live attacks on the
     * system.
     */
    char message[BUFFER_SIZE] = { 0 };
    vTaskList( message );

    /*
     * This function will violate the information leakage-related policies.
     */
    exfilToUart( message );

    /*
     * If have gotten this far, the exfiltration has succeeded.
     */
    snprintf( pcWriteBuffer, xWriteBufferLen, "<b>WARNING:</b> Data successfully exfiltrated!" );

    return 0;
}

static void exfilToUart( char *message )
{
    printf( "%s\r\n", message );
}

