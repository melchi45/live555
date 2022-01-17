/*******************************************************************************
 *  Copyright (c) 2022 Hanwha Techwin Co., Ltd.
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
 *  Security Solution Division / Smart Home Camera S/W Development Team
 *  B2C Action Camera SW Dev. Group
 *
 *  @file
 *  Contains implementation of logging tools.
 *
 *  $Author: young_ho_kim $
 *  $LastChangedBy: young_ho_kim $
 *  $Date: 2016-08-02 19:57:02 +0900 (2016-08-02, Tue) $
 *  $Revision: 2261 $
 *  $Id: StreamClientState.h 2261 2016-08-02 10:57:02Z young_ho_kim $
 *  $HeadURL: http://ctf1.stw.net/svn/repos/wearable_camera/trunk/Source/Pixcam/Wear-1.0.5/app/transport_stream/StreamClientState.h $
 *******************************************************************************

  MODULE NAME:

  REVISION HISTORY:

  Date        Ver Name                    Description
  ----------  --- --------------------- -----------------------------------------
  04-Feb-2016 0.1 Youngho Kim             Created
 ...............................................................................

  DESCRIPTION:


 ...............................................................................
 *******************************************************************************/

#ifndef STREAMCLIENTSTATE_H_
#define STREAMCLIENTSTATE_H_

#pragma once

// Define a class to hold per-stream state that we maintain throughout each stream's lifetime:
class StreamClientState {
public:
  StreamClientState();
  virtual ~StreamClientState();

public:
  MediaSubsessionIterator* iter;
  MediaSession* session;
  MediaSubsession* subsession;
  TaskToken streamTimerTask;
  double duration;
};

#endif /* STREAMCLIENTSTATE_H_ */
