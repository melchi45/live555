/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2025 Live Networks, Inc.  All rights reserved.


#ifndef _JPEG2000_VIDEO_RTP_SINK_HH
#define _JPEG2000_VIDEO_RTP_SINK_HH

#ifndef _VIDEO_RTP_SINK_HH
#include "VideoRTPSink.hh"
#endif

class LIVEMEDIA_API JPEG2000VideoRTPSink: public VideoRTPSink {
public:
  static JPEG2000VideoRTPSink* createNew(UsageEnvironment& env, Groupsock* RTPgs);

protected:
  JPEG2000VideoRTPSink(UsageEnvironment& env, Groupsock* RTPgs);
	// called only by createNew()

  virtual ~JPEG2000VideoRTPSink();

private: // redefined virtual functions:
  virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
                                      unsigned char* frameStart,
                                      unsigned numBytesInFrame,
                                      struct timeval framePresentationTime,
                                      unsigned numRemainingBytes);
  virtual unsigned specialHeaderSize() const;
};

#endif
