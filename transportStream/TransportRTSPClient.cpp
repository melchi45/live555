#include "TransportRTSPClient.hh"

TransportRTSPClient *TransportRTSPClient::createNew(UsageEnvironment &env, char const *rtspURL,
                                                    int verbosityLevel, char const *applicationName, portNumBits tunnelOverHTTPPortNum,
                                                    char *username, char *password)
{
  return new TransportRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum,
                               username, password);
}

TransportRTSPClient::TransportRTSPClient(UsageEnvironment &env, char const *rtspURL,
                                         int verbosityLevel, char const *applicationName,
                                         portNumBits tunnelOverHTTPPortNum, char *username, char *password)
    : RTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1)
{
}

TransportRTSPClient::~TransportRTSPClient()
{
}