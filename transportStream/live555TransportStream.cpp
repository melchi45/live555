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
// Copyright (c) 1996-2022, Live Networks, Inc.  All rights reserved
// LIVE555 Media Server
// main program

#include <signal.h> // for signal

#include <BasicUsageEnvironment.hh>
// #include "DynamicRTSPServer.hh"
#include "TransportRTSPClient.hh"
#include <GroupsockHelper.hh> // for "weHaveAnIPv*Address()"

#include "log_utils.h"
#include "version.hh"

#define RTSP_CLIENT_VERBOSITY_LEVEL 1 // by default, print verbose output from each "RTSPClient"
#define ACCESS_CONTROL 1

RTSPClient* transportRtspClient = NULL;

static unsigned rtspClientCount = 0; // Counts how many streams (i.e., "RTSPClient"s) are currently in use.
char* username = NULL;
char* password = NULL;
char* inputRtspUrl = NULL;
char* progName = NULL;

void usage(UsageEnvironment& env, char const* progName) {
  env << "Usage: " << progName << " <rtsp-url-1> ... <rtsp-url-N>\n";
  env << "\t(where each <rtsp-url-i> is a \"rtsp://\" URL)\n";
}

/* Allow ourselves to be shut down gracefully by a signal */
void _signalHandlerShutdown(int sig);

// Used to shut down and close a stream (including its "RTSPClient" object):
void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);

int main(int argc, char** argv) {
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

  // We need at least one "rtsp://" URL argument:
	if (argc < 2) {
		usage(*env, argv[0]);
		return 1;
	}

  while (argc > 2) {
		char* const opt = argv[1];
    progName = argv[0];
		if (opt[0] != '-')
			usage(*env, argv[0]);
		switch (opt[1]) {
    case 'i':
    {
      inputRtspUrl = argv[2];
    }
      break;
		// case 'U': { // specify start port number
		// 	bRecording = True;
		// }
		// 	break;
		case 'u': { // specify start port number
			username = argv[2];
			password = argv[3];

			*env << "Username: " << username << "\n";
			*env << "Password: " << password << "\n";

		}
			break;
		// case 'f': {
		// 	filename = argv[2];
		// 	*env << "File Name: " << filename << "\n";
		// }
		// 	break;
		default: {
		}
			break;
		}
		++argv;
		--argc;
	}

  /* Allow ourselves to be shut down gracefully by a signal */
  signal(SIGTERM, _signalHandlerShutdown);
  signal(SIGHUP, _signalHandlerShutdown);
  signal(SIGUSR1, _signalHandlerShutdown);
  signal(SIGQUIT, _signalHandlerShutdown);
  signal(SIGINT, _signalHandlerShutdown);
  signal(SIGKILL, _signalHandlerShutdown);
  signal(SIGSEGV, _signalHandlerShutdown);

  UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  InputRTSPClient* rtspClient = InputRTSPClient::createNew(*env, inputRtspUrl, username, password, RTSP_CLIENT_VERBOSITY_LEVEL, progName);
  if (rtspClient == NULL) {
    *env << "Failed to create a RTSP client for URL \"" << inputRtspUrl << "\": " << env->getResultMsg() << "\n";
    return 0;
  }

  rtspClient->sendDESCRIBE();

  ++rtspClientCount;

  // Next, send a RTSP "DESCRIBE" command, to get a SDP description for the stream.
  // Note that this command - like all RTSP commands - is sent asynchronously; we do not block, waiting for a response.
  // Instead, the following function call returns immediately, and we handle the RTSP response later, from within the event loop:
  /*rtspClient->sendDescribeCommand(continueAfterDESCRIBE);*/

  //fProxyRTSPClient
  //  = (*fCreateNewProxyRTSPClientFunc)(*this, inputStreamURL, username, password,
  //    tunnelOverHTTPPortNum,
  //    verbosityLevel > 0 ? verbosityLevel - 1 : verbosityLevel,
  //    socketNumToServer);
  //fProxyRTSPClient->sendDESCRIBE();


//   // Create the RTSP server.  Try first with the default port number (554),
//   // and then with the alternative port number (8554):
//   RTSPServer* rtspServer;
//   portNumBits rtspServerPortNum = 554;
//   rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
//   if (rtspServer == NULL) {
//     rtspServerPortNum = 8554;
//     rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
//   }
//   if (rtspServer == NULL) {
//     *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
//     exit(1);
//   }

//   *env << "LIVE555 Media Server\n";
//   *env << "\tversion " << MEDIA_SERVER_VERSION_STRING
//        << " (LIVE555 Streaming Media library version "
//        << LIVEMEDIA_LIBRARY_VERSION_STRING << ").\n";

//   *env << "Play streams from this server using the URL\n";
//   if (weHaveAnIPv4Address(*env)) {
//     char* rtspURLPrefix = rtspServer->ipv4rtspURLPrefix();
//     *env << "\t" << rtspURLPrefix << "<filename>\n";
//     delete[] rtspURLPrefix;
//     if (weHaveAnIPv6Address(*env)) *env << "or\n";
//   }
//   if (weHaveAnIPv6Address(*env)) {
//     char* rtspURLPrefix = rtspServer->ipv6rtspURLPrefix();
//     *env << "\t" << rtspURLPrefix << "<filename>\n";
//     delete[] rtspURLPrefix;
//   }
//   *env << "where <filename> is a file present in the current directory.\n";

//   *env << "Each file's type is inferred from its name suffix:\n";
//   *env << "\t\".264\" => a H.264 Video Elementary Stream file\n";
//   *env << "\t\".265\" => a H.265 Video Elementary Stream file\n";
//   *env << "\t\".aac\" => an AAC Audio (ADTS format) file\n";
//   *env << "\t\".ac3\" => an AC-3 Audio file\n";
//   *env << "\t\".amr\" => an AMR Audio file\n";
//   *env << "\t\".dv\" => a DV Video file\n";
//   *env << "\t\".m4e\" => a MPEG-4 Video Elementary Stream file\n";
//   *env << "\t\".mkv\" => a Matroska audio+video+(optional)subtitles file\n";
//   *env << "\t\".mp3\" => a MPEG-1 or 2 Audio file\n";
//   *env << "\t\".mpg\" => a MPEG-1 or 2 Program Stream (audio+video) file\n";
//   *env << "\t\".ogg\" or \".ogv\" or \".opus\" => an Ogg audio and/or video file\n";
//   *env << "\t\".ts\" => a MPEG Transport Stream file\n";
//   *env << "\t\t(a \".tsx\" index file - if present - provides server 'trick play' support)\n";
//   *env << "\t\".vob\" => a VOB (MPEG-2 video with AC-3 audio) file\n";
//   *env << "\t\".wav\" => a WAV Audio file\n";
//   *env << "\t\".webm\" => a WebM audio(Vorbis)+video(VP8) file\n";
//   *env << "See http://www.live555.com/mediaServer/ for additional documentation.\n";

//   // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
//   // Try first with the default HTTP port (80), and then with the alternative HTTP
//   // port numbers (8000 and 8080).

//   if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
//     *env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling).)\n";
//   } else {
//     *env << "(RTSP-over-HTTP tunneling is not available.)\n";
//   }

//   env->taskScheduler().doEventLoop(); // does not return

  return 0; // only to prevent compiler warning
}

void _signalHandlerShutdown(int sig)
{
  log_error("%s: Got signal %d, program exits!", __FUNCTION__, sig);

  if(transportRtspClient != nullptr) {
    shutdownStream(transportRtspClient);
  }

/*
    if(env != NULL) {
    	env->reclaim();
    	env = NULL;
    }

    if(scheduler != NULL) {
        delete scheduler;
        scheduler = NULL;
    }

    if( SIGSEGV == sig )
        exit(-1);
*/

    return;
}

void shutdownStream(RTSPClient* rtspClient, int exitCode) {
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((TransportRTSPClient*)rtspClient)->scs; // alias

  // // First, check whether any subsessions have still to be closed:
  // if (scs.session != NULL)
  // {
  //   Boolean someSubsessionsWereActive = False;
  //   MediaSubsessionIterator iter(*scs.session);
  //   MediaSubsession *subsession;

  //   while ((subsession = iter.next()) != NULL)
  //   {
  //     if (subsession->sink != NULL)
  //     {
  //       Medium::close(subsession->sink);
  //       subsession->sink = NULL;

  //       if (subsession->rtcpInstance() != NULL)
  //       {
  //         subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
  //       }

  //       someSubsessionsWereActive = True;
  //     }
  //   }

  //   if (someSubsessionsWereActive)
  //   {
  //     // Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
  //     // Don't bother handling the response to the "TEARDOWN".
  //     rtspClient->sendTeardownCommand(*scs.session, NULL);
  //   }
  // }

  // env << *rtspClient << "Closing the stream.\n";
  // Medium::close(rtspClient);
  // Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.

  if (--rtspClientCount == 0)
  {
    // The final stream has ended, so exit the application now.
    // (Of course, if you're embedding this code into your own application, you might want to comment this out,
    // and replace it with "eventLoopWatchVariable = 1;", so that we leave the LIVE555 event loop, and continue running "main()".)
    exit(exitCode);
  }
}