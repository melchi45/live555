#ifndef TRANSPORTRTSPCLIENT_H
#define TRANSPORTRTSPCLIENT_H
#pragma once

#include "liveMedia.hh"
#include "InputRTSPClient.hh"
#include "StreamClientState.hh"

class TransportRTSPClient : public RTSPClient{
public:
	static TransportRTSPClient *createNew(UsageEnvironment &env, char const *rtspURL,
																				int verbosityLevel = 0,
																				char const *applicationName = NULL,
																				portNumBits tunnelOverHTTPPortNum = 0, char *username = NULL, char *password = NULL);

protected:
	TransportRTSPClient(UsageEnvironment &env, char const *rtspURL,
											int verbosityLevel, char const *applicationName, portNumBits tunnelOverHTTPPortNum,
											char *username = NULL, char *password = NULL);
	virtual ~TransportRTSPClient();

public:
  StreamClientState scs;

protected:
	InputRTSPClient* inputRTSPClient;
};
#endif