#include "InputRTSPClient.hh"

InputRTSPClient* InputRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
          char const* username, char const* password,
					int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) {
  return new InputRTSPClient(env, rtspURL, username, password, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

InputRTSPClient::InputRTSPClient(UsageEnvironment& env, char const* rtspURL,
           char const* username, char const* password,
			     int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
  : RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1)
  , fOurAuthenticator(NULL) {
  if (username != NULL && password != NULL) {
    fOurAuthenticator = new Authenticator(username, password);
  }
}

InputRTSPClient::~InputRTSPClient() {
  delete fOurAuthenticator;
}

void InputRTSPClient::sendDESCRIBE(void* clientData) {
  InputRTSPClient* rtspClient = (InputRTSPClient*)clientData;
  if (rtspClient != NULL) {
    rtspClient->fDESCRIBECommandTask = NULL;
    rtspClient->sendDESCRIBE();
  }
}


///////// RTSP 'response handlers' //////////

static void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) {
  char const* res;

  if (resultCode == 0) {
    // The "DESCRIBE" command succeeded, so "resultString" should be the stream's SDP description.
    res = resultString;
  }
  else {
    // The "DESCRIBE" command failed.
    res = NULL;
  }
  ((ProxyRTSPClient*)rtspClient)->continueAfterDESCRIBE(res);
  delete[] resultString;
}

void InputRTSPClient::sendDESCRIBE() {
  sendDescribeCommand(::continueAfterDESCRIBE, fOurAuthenticator);
}

void InputRTSPClient::continueAfterDESCRIBE(char const* sdpDescription) {
  UsageEnvironment& env = this->envir(); // alias
  env << "Got a SDP description:\n" << sdpDescription << "\n";

  //if (sdpDescription != NULL) {
  //  fOurServerMediaSession.continueAfterDESCRIBE(sdpDescription);

  //  // Unlike most RTSP streams, there might be a long delay between this "DESCRIBE" command (to the downstream server) and the
  //  // subsequent "SETUP"/"PLAY" - which doesn't occur until the first time that a client requests the stream.
  //  // To prevent the proxied connection (between us and the downstream server) from timing out, we send periodic 'liveness'
  //  // ("OPTIONS" or "GET_PARAMETER") commands.  (The usual RTCP liveness mechanism wouldn't work here, because RTCP packets
  //  // don't get sent until after the "PLAY" command.)
  //  scheduleLivenessCommand();
  //}
  //else {
  //  // The "DESCRIBE" command failed, most likely because the server or the stream is not yet running.
  //  // Reschedule another "DESCRIBE" command to take place later:
  //  scheduleDESCRIBECommand();
  //}
  //fDoneDESCRIBE = True;
}