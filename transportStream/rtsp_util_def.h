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
 *  Security Solution Division / Camera S/W Development Team
 *  Home Security Camera SW Dev. Group
 *
 *  @file
 *  Contains implementation of logging tools.
 *
 *  $Author: young_ho_kim $
 *  $LastChangedBy: young_ho_kim $
 *  $Date: 2016-08-02 21:06:33 +0900 (화, 02 8 2016) $
 *  $Revision: 2267 $
 *  $Id: rtsp_util_def.h 2267 2016-08-02 12:06:33Z young_ho_kim $
 *  $HeadURL: http://ctf1.stw.net/svn/repos/wearable_camera/trunk/Source/Pixcam/Wear-1.0.5/app/live555_server/rtsp_util_def.h $
 *******************************************************************************

  MODULE NAME:

  REVISION HISTORY:

  Date        Ver Name                    Description
  ----------  --- --------------------- -----------------------------------------
  07-Jun-2016 0.1 Youngho Kim             Created
 ...............................................................................

  DESCRIPTION:


 ...............................................................................
 *******************************************************************************/

#ifndef __util_def_H_wltTx9Yg_ljkr_HUV4_s1wM_umMiphhGWn41__
#define __util_def_H_wltTx9Yg_ljkr_HUV4_s1wM_umMiphhGWn41__

#include <stdio.h>
#include "log_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Constant Definition
//=============================================================================

//=============================================================================
//                Macro Definition
//=============================================================================
#define _toString(x)    #x
#define toStr(x)        _toString(x)

#if (1)
    #define err_msg(string, args...)        do{ log_error("[#%d]%s => ", __LINE__, __FUNCTION__);\
                                                log_error(string, ## args); \
                                            }while(0)

    #define dbg_msg(string, args...)        log_info(string, ## args);

    #define dbg_trace(string, args...)
    #define trace()

#elif defined(__STDC_VERSION__) // DEBUG
    #define err_msg(string, args...)        do{ fprintf(stderr, "[#%d]%s => ", __LINE__, __PRETTY_FUNCTION__);\
                                                fprintf(stderr, string, ## args); \
                                            }while(0)
    #define dbg_trace(string, args...)      //fprintf(stderr, "[#%d]%s\n", __LINE__, __PRETTY_FUNCTION__)

    #define dbg_msg(string, args...)        do{ fprintf(stderr, "[#%d]%s => ", __LINE__, __PRETTY_FUNCTION__);\
                                                fprintf(stderr, string, ## args); \
                                            }while(0)
    #define trace()                         fprintf(stderr, "[#%d]%s\n", __LINE__, __PRETTY_FUNCTION__)
#else /* #if defined(__STDC_VERSION__) */
    #if defined(__GNUC__)
    #define err_msg(string, args...)        do{ fprintf(stderr, "%s [#%d] => ", __FUNCTION__, __LINE__);\
                                                fprintf(stderr, string, ## args); \
                                            }while(0)
    #define dbg_trace(string, args...)      fprintf(stderr, "%s [#%d]\n", __FUNCTION__, __LINE__)

    #define dbg_msg(string, ...)            do{ fprintf(stderr, "%s [#%d] => ", __FUNCTION__, __LINE__);\
                                                fprintf(stderr, string, ## args); \
                                            }while(0)
    #define trace()                         fprintf(stderr, "%s [#%d]\n", __FUNCTION__, __LINE__)
    #else /** #if defined(__GNUC__) **/
    #define err_msg(string, ...)            do{ fprintf(stderr, "%s [#%d] => ", __FUNCTION__, __LINE__);\
                                                fprintf(stderr, string, __VA_ARGS__); \
                                            }while(0)
    #define dbg_trace(string, ...)          fprintf(stderr, "%s [#%d]\n", __FUNCTION__, __LINE__)

    #define dbg_msg                         do{ fprintf(stderr, "%s [#%d] => ", __FUNCTION__, __LINE__);\
                                                fprintf(stderr, string, __VA_ARGS__); \
                                            }while(0)
    #define trace()                         fprintf(stderr, "%s [#%d]\n", __FUNCTION__, __LINE__)
    #endif /** #if defined(__GNUC__) **/
#endif /* #if defined(__STDC_VERSION__) */

//=============================================================================
//                Structure Definition
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================

#ifdef __cplusplus
}
#endif

#endif
