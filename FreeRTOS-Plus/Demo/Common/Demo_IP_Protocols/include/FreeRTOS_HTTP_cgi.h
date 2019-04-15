/*
 * File: FreeRTOS_HTTP_cgi.h
 *
 * Description:
 *   This file defines the HTTP server CGI interface.
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

#ifndef HTTP_CGI_H
#define HTTP_CGI_H

/* FreeRTOS includes */
#include "FreeRTOS.h"

/* Settings */
#define CGI_NUM_CGIS_MAX 32
#define CGI_REST_ARGS_DELIM_START "ARGS"
#define CGI_REST_ARGS_DELIM_END "ENDARGS"
#define CGI_RESPONSE_LEN_MAX 2048
#define CGI_ARGS_LEN_MAX 128
#define CGI_HEADER_LEN_MAX 256

/* Types */
typedef BaseType_t ( *pdCGI_CALLBACK)( char *pcWriteBuffer, size_t xWriteBufferLen,
                                       char *pcHeaderBuffer, size_t xHeaderBufferLen,
                                       const char *pcCgiArgs );

typedef struct xCGI_REGISTRY_ENTRY
{
    const char *const pcCgiUrl;
    pdCGI_CALLBACK pxCgiFunc;
} CGI_Registry_Entry_t;

/* Function declarations */

/*
 * Register the CGI passed in using the 'cgi' parameter. Registering a CGI adds the CGI to the list
 * of CGIs that are handled by the HTTP server. Once a CGI has been registered it can be executed by
 * HTTP clients via the POST request.
 */
BaseType_t FreeRTOS_CGIRegister( const CGI_Registry_Entry_t *const pxCgi );

/*
 * Parse the HTTP REST string for the CGI arguments.
 */
BaseType_t FreeRTOS_CGIArgsGet( const char *pcRestData, char *pcArgs, size_t szArgsLen );

/*
 * Execute the CGI identified by the url in the 'pcUrl' paramter. Returns 0 on success or a WEB
 * code on failure.
 */
BaseType_t FreeRTOS_CGIExec( const char *const pcUrl, char *pcWriteBuffer, size_t xWriteBufferLen,
                             const char *pcCgiArgs );

#endif /* HTTP_CGI_H */

