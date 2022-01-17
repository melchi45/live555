/*******************************************************************************
 *  Copyright (c) 2016 Hanwha Techwin Co., Ltd.
 *
 *  Licensed to the Hanwha Techwin Software Foundation under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  Smart Home Camera - Hanwha B2C Action Cam Project
 *  http://www.samsungsmartcam.com
 *
 *  Security Solution Division / Smart Home Camera SW Dev. Group
 *  B2C Action Camera SW Dev. Group
 *
 *  @file
 *  Contains implementation of logging tools.
 *
 *  $Author: young_ho_kim $
 *  $LastChangedBy: young_ho_kim $
 *  $Date: 2016-08-19 16:44:07 +0900 (Fri, 19 Aug 2016) $
 *  $Revision: 2352 $
 *  $Id: log_utils.c 2352 2016-08-19 07:44:07Z young_ho_kim $
 *  $HeadURL: http://ctf1.stw.net/svn/repos/wearable_camera/trunk/Source/Pixcam/Wear-1.0.5/app/dm/log/log_utils.c $
 *******************************************************************************

  MODULE NAME:

  REVISION HISTORY:

  Date        Ver Name                    Description
  ----------  --- --------------------- -----------------------------------------
  04-Jun-2016 0.1 Youngho Kim             Created
 ...............................................................................

  DESCRIPTION:


 ...............................................................................
 *******************************************************************************/

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

#include "bool.h"
#include "log_utils.h"

/** @name Logging to @a stdout
 * @{ */
static void log_rtspPrintf(char* fmt, ...);
static void log_infoPrintf(char* fmt, ...);
static void log_debugPrintf(char* fmt, ...);
static void log_warningPrintf(char* fmt, ...);
static void log_errorPrintf(char* fmt, ...);
/** @}
 * @name Logging using @a syslog
 * @{ */
static void log_rtspSyslog(char* fmt, ...);
static void log_infoSyslog(char* fmt, ...);
static void log_debugSyslog(char* fmt, ...);
static void log_warningSyslog(char* fmt, ...);
static void log_errorSyslog(char* fmt, ...);
/** @} */

/** @name Log handlers.
 * Used to invoke quickly current logging function.
 * @{ */
log_function log_rtspHandler = &log_rtspPrintf;
log_function log_infoHandler = &log_infoPrintf;
log_function log_debugHandler = &log_debugPrintf;
log_function log_warningHandler = &log_warningPrintf;
log_function log_errorHandler = &log_errorPrintf;
/** @} */

/** Defines global log level. */
static int _log_level = LOG_LEVEL_DEBUG;

/** Logging mode. */
static BOOL _use_syslog = FALSE;

/** Logs info message using printf. */
static void log_rtspPrintf(char* fmt, ...)
{
    printf(ANSI_COLOR_CYAN "[%5.6f TRANSPORT_STREAM:INFO] ", ((double)clock())/CLOCKS_PER_SEC);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf(ANSI_COLOR_RESET "\n");
}

/** Logs info message using printf. */
static void log_infoPrintf(char* fmt, ...)
{
    printf(ANSI_COLOR_GREEN "[%5.6f TRANSPORT_STREAM:INFO] ", ((double)clock())/CLOCKS_PER_SEC);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf(ANSI_COLOR_RESET "\n");
}

/** Logs debug message using printf. */
static void log_debugPrintf(char* fmt, ...)
{
    printf(ANSI_COLOR_BLUE "[%5.6f TRANSPORT_STREAM:DEBUG] ", ((double)clock())/CLOCKS_PER_SEC);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf(ANSI_COLOR_RESET "\n");
}

/** Logs warning message using printf. */
static void log_warningPrintf(char* fmt, ...)
{
    printf(ANSI_COLOR_MAGENTA "[%5.6f TRANSPORT_STREAM:WARNING] ", ((double)clock())/CLOCKS_PER_SEC);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf(ANSI_COLOR_RESET "\n");
}

/** Logs error message using printf. */
static void log_errorPrintf(char* fmt, ...)
{
    printf(ANSI_COLOR_RED "[%5.6f TRANSPORT_STREAM:ERROR] ",  ((double)clock())/CLOCKS_PER_SEC);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf(ANSI_COLOR_RESET"\n");
}

/** Logs warning message using syslog. */
static void log_rtspSyslog(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsyslog(LOG_INFO, fmt, args);
    va_end(args);
}

/** Logs warning message using syslog. */
static void log_infoSyslog(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsyslog(LOG_INFO, fmt, args);
    va_end(args);
}

/** Logs debug message using syslog. */
static void log_debugSyslog(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsyslog(LOG_DEBUG, fmt, args);
    va_end(args);
}

/** Logs warning message using syslog. */
static void log_warningSyslog(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsyslog(LOG_WARNING, fmt, args);
    va_end(args);
}

/** Logs error message using syslog. */
static void log_errorSyslog(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsyslog(LOG_ERR, fmt, args);
    va_end(args);
}

/** Does not log anything.
 * Used to ignore messages of some priorities according to the global log level.
 */
static void log_nothing(char* fmt, ...)
{
    // dismiss the log message
    va_list args;
    va_start(args, fmt);
//    vsyslog(LOG_ERR, fmt, args);
    va_end(args);
}

/** Configures logging system in printf mode according to global log level. */
static void setPrintf()
{
    // drop all log functions
	log_rtspHandler = &log_nothing;
	log_infoHandler = &log_nothing;
    log_debugHandler = &log_nothing;
    log_warningHandler = &log_nothing;
    log_errorHandler = &log_nothing;
    // set corresponding log functions
    switch(_log_level)
    {
    case LOG_LEVEL_DEBUG:
        log_debugHandler = &log_debugPrintf;
    case LOG_LEVEL_WARNING:
        log_warningHandler = &log_warningPrintf;
    case LOG_LEVEL_RTSP:
        log_rtspHandler = &log_rtspPrintf;
    case LOG_LEVEL_INFO:
        log_infoHandler = &log_infoPrintf;
    case LOG_LEVEL_ERROR:
        log_errorHandler = &log_errorPrintf;
    default:
    	break;
    }
}

/** Configures logging system in syslog mode according to global log level. */
static void setSyslog()
{
    // drop all log functions
	log_rtspHandler = &log_nothing;
	log_infoHandler = &log_nothing;
    log_debugHandler = &log_nothing;
    log_warningHandler = &log_nothing;
    log_errorHandler = &log_nothing;
    // set corresponding log functions
    switch(_log_level)
    {
    case LOG_LEVEL_DEBUG:
        log_debugHandler = &log_debugSyslog;
    case LOG_LEVEL_WARNING:
        log_warningHandler = &log_warningSyslog;
    case LOG_LEVEL_RTSP:
        log_rtspHandler = &log_rtspSyslog;
    case LOG_LEVEL_INFO:
        log_infoHandler = &log_infoSyslog;
    case LOG_LEVEL_ERROR:
        log_errorHandler = &log_errorSyslog;
    default:
    	break;
    }
}

void log_usePrintf()
{
    _use_syslog = FALSE;
    // close syslog connection if it was opened
    closelog();
    setPrintf();
}

void log_useSyslog(int facility)
{
    _use_syslog = TRUE;
    openlog(NULL, LOG_NDELAY, facility);
    setSyslog();
}

void log_closeSyslog()
{
	// close syslog connection if it was opened
	closelog();
}

void log_setLevel(LOG_LEVEL level)
{
    _log_level = level;
    if (_use_syslog)
    {
        setSyslog();
    }
    else
    {
        setPrintf();
    }
}
