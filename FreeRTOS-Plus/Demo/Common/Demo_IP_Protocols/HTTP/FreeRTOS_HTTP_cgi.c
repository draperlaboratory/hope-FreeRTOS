/*
 * File: FreeRTOS_HTTP_cgi.c
 *
 * Description:
 *   This file includes the HTTP server CGI implementations.
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

/* FreeRTOS+TCP includes. */
#include "FreeRTOSIPConfig.h"
#include "FreeRTOS_HTTP_commands.h"

/* FreeRTOS Protocol includes. */
#include "FreeRTOS_HTTP_cgi.h"

/* Local defines */
#define CGI_XML_START "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
#define CGI_OUTPUT_START "<cgiOutput>"
#define CGI_OUTPUT_END "</cgiOutput>"
#define CGI_RESPONSE_TEXT_START "<responseText>"
#define CGI_RESPONSE_TEXT_END   "</responseText>"
#define CGI_HEADER_FIELDS_START "<headerfields>"
#define CGI_HEADER_FIELDS_END   "</headerfields>"
#define CGI_EXIT_STATUS_START "<exitstatus>"
#define CGI_EXIT_STATUS_END   "</exitstatus>"
#define MAX_REST_DATA_LEN 2048

/* Local function declarations */
static BaseType_t prvCgiLookup( const char *pcUrl, CGI_Registry_Entry_t **pxCgi );

/* Local data */
static CGI_Registry_Entry_t CgiRegistry[CGI_NUM_CGIS_MAX] = { 0 };
static int CgiCount = 0;

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/* Local function definitions */
static BaseType_t prvCgiLookup( const char *pcUrl, CGI_Registry_Entry_t **pxCgi )
{
BaseType_t xRc = 1;

    for( int i = 0; i < CgiCount; i++ )
    {
        if( 0 == strcmp( pcUrl, CgiRegistry[i].pcCgiUrl ) )
        {
            if( NULL != pxCgi )
            {
                (*pxCgi) = &CgiRegistry[i];
            }
            xRc = 0;
            break;
        }
    }

    return xRc;
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/* External function definitions */
BaseType_t FreeRTOS_CGIRegister( const CGI_Registry_Entry_t *const pxCgi )
{
BaseType_t xRc = 1;

    if( CgiCount < CGI_NUM_CGIS_MAX )
    {
        if( 0 != prvCgiLookup( pxCgi->pcCgiUrl, NULL ) )
        {
            memcpy( &CgiRegistry[CgiCount], pxCgi, sizeof( CGI_Registry_Entry_t ) );
            CgiCount++;
            xRc = 0;
        }
    }

    return xRc;
}
/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_CGIArgsGet( const char *pcRestData, char *pcArgs, size_t szArgsLen )
{
BaseType_t xRc = 0;
const char pcArgsStartDelim[] = CGI_REST_ARGS_DELIM_START;
const char pcArgsEndDelim[] = CGI_REST_ARGS_DELIM_END;
char pcRestDataFixed[MAX_REST_DATA_LEN] = { 0 };

    pcArgs[0] = '\0';

    strncpy( pcRestDataFixed, pcRestData, sizeof(pcRestDataFixed) );
    pcRestDataFixed[MAX_REST_DATA_LEN - 1] = '\0';

    char *pcArgsStart = strstr( pcRestDataFixed, pcArgsStartDelim );
    if( NULL != pcArgsStart )
    {
        /* If have start delimiter but not end, then have malformed request */
        xRc = WEB_BAD_REQUEST;

        /* Find last instance of CGI_REST_ARGS_DELIM_END */
        char *pcArgsEnd = NULL;
        char *pcNextEndDelim = pcRestDataFixed;
        do
        {
            pcArgsEnd = pcNextEndDelim;
            pcNextEndDelim = strstr( pcNextEndDelim + 1, pcArgsEndDelim );
        } while( NULL != pcNextEndDelim );

        if( NULL != pcArgsEnd )
        {
            /* Have args start and end; now put into pcArgs */
            char *pcStartAddr = &pcArgsStart[strlen( pcArgsStartDelim )];
            size_t szRestArgsLen = (size_t)(pcArgsEnd - pcStartAddr);
            size_t szMinArgsLen = (szRestArgsLen < szArgsLen) ? szRestArgsLen : szArgsLen;

            strncpy( pcArgs, pcStartAddr, szMinArgsLen );
            pcArgs[szArgsLen - 1] = '\0';
            xRc = 0;
        }
    }

    return xRc;
}
/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_CGIExec( const char *const pcUrl, char *pcResponse, size_t xResponseLen,
                             const char *pcCgiArgs )
{
BaseType_t xRc = 0;
BaseType_t xHTTPResponseCode;
CGI_Registry_Entry_t *pxCgi = NULL;
char pcHeaderFields[CGI_HEADER_LEN_MAX];
int currIndex;

    xRc = prvCgiLookup( pcUrl, &pxCgi );

    if( 0 == xRc )
    {
        printf( "CGI lookup found for url: %s\n", pcUrl );
        /* Basic HTML start */
        currIndex = snprintf( pcResponse, xResponseLen, CGI_XML_START );
        currIndex += snprintf(&pcResponse[currIndex], xResponseLen - currIndex, CGI_OUTPUT_START);

        currIndex += snprintf(&pcResponse[currIndex], xResponseLen - currIndex, CGI_RESPONSE_TEXT_START);
        /* Exec the CGI */
        xHTTPResponseCode = pxCgi->pxCgiFunc( &pcResponse[currIndex], xResponseLen - currIndex, pcHeaderFields, CGI_HEADER_LEN_MAX, pcCgiArgs );

        if (xHTTPResponseCode == WEB_INTERNAL_SERVER_ERROR) {
          xRc = xHTTPResponseCode;
        }

        /* Basic HTML end */
        currIndex = strnlen( pcResponse, xResponseLen );

        currIndex += snprintf( &pcResponse[currIndex], xResponseLen - currIndex, CGI_RESPONSE_TEXT_END );

        currIndex += snprintf( &pcResponse[currIndex], xResponseLen - currIndex, CGI_HEADER_FIELDS_START );
        currIndex += snprintf( &pcResponse[currIndex], xResponseLen - currIndex, pcHeaderFields );
        currIndex += snprintf( &pcResponse[currIndex], xResponseLen - currIndex, CGI_HEADER_FIELDS_END );

        currIndex += snprintf( &pcResponse[currIndex], xResponseLen - currIndex, CGI_EXIT_STATUS_START );
        currIndex += snprintf( &pcResponse[currIndex], xResponseLen - currIndex, "%d", xHTTPResponseCode );
        currIndex += snprintf( &pcResponse[currIndex], xResponseLen - currIndex, CGI_EXIT_STATUS_END );

        currIndex += snprintf( &pcResponse[currIndex], xResponseLen - currIndex, CGI_OUTPUT_END );

        printf("Response code: %d\n", xHTTPResponseCode);
        /* printf( "CGI return text: \n\t%s", pcResponse ); */
    }
    else
    {
        printf( "CGI lookup not found for url: %s", pcUrl );
        xRc = WEB_NOT_FOUND;
    }

    return xRc;
}
/*-----------------------------------------------------------*/


