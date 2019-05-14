/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * This file defines some very basic web content that is used as default web
 * pages when the HTTP server is created.  A FAT file is created in the
 * directory set by the configHTTP_ROOT constant (in FreeRTOSConfig.h) from each
 * file definition below.  The files were converted to C structures using Hex
 * Edit (http://www.hexedit.com).
 */

/* A structure that defines a single file to be copied to the RAM disk. */
typedef struct FILE_TO_COPY
{
	const char *pcFileName; /* The name of the file. */
	size_t xFileSize;
	const uint8_t *pucFileData; /* The C structure that contains the file's data. */
} xFileToCopy_t;

#include "generated_files.h"

const xFileToCopy_t xHTTPFilesToCopy[] =
{
	{ "add-record.html", sizeof( add_record_html ), add_record_html },
	{ "dashboard.html", sizeof( dashboard_html ), dashboard_html },
	{ "login.html", sizeof( login_html ), login_html },
	{ "logout.html", sizeof( logout_html ), logout_html },
	{ "index.html", sizeof( index_html ), index_html },
	{ "register.html", sizeof( register_html ), register_html },
	{ "search.html", sizeof( search_html ), search_html },
	{ "search-results.html", sizeof( search_results_html ), search_results_html },
	{ "update-user.html", sizeof( update_user_html ), update_user_html },
	{ "user-details.html", sizeof( user_details_html ), user_details_html },
	{ "helpers.js", sizeof( helpers_js ), helpers_js },
};

