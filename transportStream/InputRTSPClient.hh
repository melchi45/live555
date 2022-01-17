#ifndef INPUTRTSPCLIENT_H
#define INPUTRTSPCLIENT_H
#pragma once

#include "liveMedia.hh"
#include "StreamClientState.hh"

class InputRTSPClient : public RTSPClient {
public:
  static InputRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
					char const* username = NULL,
					char const* password = NULL,
				  int verbosityLevel = 0,
				  char const* applicationName = NULL,
				  portNumBits tunnelOverHTTPPortNum = 0);

	InputRTSPClient(UsageEnvironment& env, char const* rtspURL,
		char const* username, char const* password,
		int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
	virtual ~InputRTSPClient();

	void continueAfterDESCRIBE(char const* sdpDescription);

public:
  StreamClientState scs;
	void sendDESCRIBE();
private:
	Authenticator* auth() { return fOurAuthenticator; }

	static void sendDESCRIBE(void* clientData);


private:
	Authenticator* fOurAuthenticator;
	TaskToken fLivenessCommandTask, fDESCRIBECommandTask, fSubsessionTimerTask, fResetTask;
};
#endif