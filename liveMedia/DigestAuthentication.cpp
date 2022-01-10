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
// Copyright (c) 1996-2022 Live Networks, Inc.  All rights reserved.
// A class used for digest authentication.
// Implementation

#include "DigestAuthentication.hh"
#include "ourMD5.hh"
#include "SHA256.hh"
#include <strDup.hh>
#include <GroupsockHelper.hh> // for gettimeofday()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Authenticator::Authenticator() {
  assign(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, False, -1);
}

Authenticator::Authenticator(char const* username, char const* password, Boolean passwordIsMD5) {
  assign(NULL, NULL, username, password, NULL, NULL, NULL, NULL, NULL, passwordIsMD5, -1);
}

Authenticator::Authenticator(const Authenticator& orig) {
  assign(orig.realm(), orig.nonce(), orig.username(), orig.password(),
    orig.opaque(), orig.algorithm(), orig.qop(), orig.cnonce(), orig.nc(), orig.fPasswordIsMD5, -1);
}

Authenticator& Authenticator::operator=(const Authenticator& rightSide) {
  if (&rightSide != this) {
    reset();
    assign(rightSide.realm(), rightSide.nonce(),
	   rightSide.username(), rightSide.password(), 
           rightSide.opaque(), rightSide.algorithm(), rightSide.qop(), 
           rightSide.cnonce(), rightSide.nc(), rightSide.fPasswordIsMD5,
           rightSide.typeOfHash());
  }

  return *this;
}

Boolean Authenticator::operator<(const Authenticator* rightSide) {
  // Returns True if "rightSide" is 'newer' than us:
  if (rightSide != NULL && rightSide != this &&
      (rightSide->realm() != NULL || rightSide->nonce() != NULL ||
       username() == NULL || password() == NULL ||
       strcmp(rightSide->username(), username()) != 0 ||
       strcmp(rightSide->password(), password()) != 0)) {
    return True;
  }

  return False;
}

Authenticator::~Authenticator() {
  reset();
}

void Authenticator::reset() {
  resetRealmAndNonce();
  resetUsernameAndPassword();
  resetOpaqueAndAlgorithmAndQop();
  resetCnonceAndNonceCount();
	resetHashType();
}

void Authenticator::setRealmAndNonce(char const* realm, char const* nonce) {
  resetRealmAndNonce();
  assignRealmAndNonce(realm, nonce);
}

void Authenticator::setRealmAndRandomNonce(char const* realm) {
  resetRealmAndNonce();

  // Construct data to seed the random nonce:
  struct {
    struct timeval timestamp;
    unsigned counter;
  } seedData;
  gettimeofday(&seedData.timestamp, NULL);
  static unsigned counter = 0;
  seedData.counter = ++counter;

  // Use MD5 to compute a 'random' nonce from this seed data:
  char nonceBuf[33];
  our_MD5Data((unsigned char*)(&seedData), sizeof seedData, nonceBuf);

  assignRealmAndNonce(realm, nonceBuf);
}

void Authenticator::setUsernameAndPassword(char const* username,
					   char const* password,
					   Boolean passwordIsMD5) {
  resetUsernameAndPassword();
  assignUsernameAndPassword(username, password, passwordIsMD5);
}

void Authenticator::setHashType(int hashType) {
	resetHashType();
	assignHashType(hashType);
}

void Authenticator::setOpaqueAndAlgorithmAndQop(char const* opaque, char const* algorithm, char const* qop) {
  resetOpaqueAndAlgorithmAndQop();
  assignOpaqueAndAlgorithmAndQop(opaque, algorithm, qop);

	// Construct data to seed the random nonce:
	struct {
		struct timeval timestamp;
		unsigned counter;
	} seedData;
	gettimeofday(&seedData.timestamp, NULL);
	static unsigned counter = 0;
	seedData.counter = ++counter;

	// Use MD5 to compute a 'random' nonce from this seed data:
	char nonceBuf[33];
	our_MD5Data((unsigned char*)(&seedData), sizeof seedData, nonceBuf);

	char nc[9];
	sprintf(nc, "%08d", counter);

	assignCnonceAndNonceCount(nonceBuf, nc);

}

char const* Authenticator::computeDigestResponse(char const* cmd,
						 char const* url) const {
  // The "response" field is computed as:
  //    md5(md5(<username>:<realm>:<password>):<nonce>:md5(<cmd>:<url>))
  // or, if "fPasswordIsMD5" is True:
  //    md5(<password>:<nonce>:md5(<cmd>:<url>))
	if (fAlgorithm != NULL) {
		if (strcmp(fAlgorithm, "SHA-256") == 0) {
			char ha1Buf[65];
			char entityhex[65];
			//util::SHA256 sha256;
			if (fPasswordIsMD5) {
				strncpy(ha1Buf, password(), 64);
				ha1Buf[64] = '\0'; // just in case
			} else {
				unsigned const ha1DataLen = strlen(username()) + 1
					+ strlen(realm()) + 1 + strlen(password());
				unsigned char* ha1Data = new unsigned char[ha1DataLen + 1];
				sprintf((char*)ha1Data, "%s:%s:%s", username(), realm(), password());
				char const* ha1temp = sha256_hash((char*)ha1Data);
				sprintf((char*)ha1Buf, "%s", ha1temp);
				free((char*)ha1temp);
				delete[] ha1Data;
			}

			unsigned ha2DataLen = 0;
			unsigned char* ha2Data = NULL;
			if (strcmp(fQop, "auth-int") == 0)
			{
				size_t n = sizeof(entityhex) / sizeof(entityhex[0]);
				ha2DataLen = strlen(cmd) + 1 + strlen(url) + 1 + n;
				ha2Data = new unsigned char[ha2DataLen + 1];
				sprintf((char*)ha2Data, "%s:%s:%s", cmd, url, entityhex);
			}
			else {
				ha2DataLen = strlen(cmd) + 1 + strlen(url);
				ha2Data = new unsigned char[ha2DataLen + 1];
				sprintf((char*)ha2Data, "%s:%s", cmd, url);
			}

			char ha2Buf[65];
			char const* ha2temp = sha256_hash((char*)ha2Data);
			sprintf((char*)ha2Buf, "%s", ha2temp);
			free((char*)ha2temp);
			delete[] ha2Data;

			unsigned digestDataLen = 0;
			unsigned char* digestData = NULL;
			if (strcmp(fQop, "auth") == 0) {
				digestDataLen = 64 + 1 + strlen(nonce()) + 1
					+ strlen(nc()) + 1 + strlen(cnonce()) + 1 + strlen(qop()) + 1 + 64;
				digestData = new unsigned char[digestDataLen + 1];
				sprintf((char*)digestData, "%s:%s:%s:%s:%s:%s",
					ha1Buf, nonce(),
					nc(), cnonce(), qop(), ha2Buf);
			}
			else {
				digestDataLen = 64 + 1 + strlen(nonce()) + 1 + 64;
				digestData = new unsigned char[digestDataLen + 1];
				sprintf((char*)digestData, "%s:%s:%s",
					ha1Buf, nonce(), ha2Buf);
			}

			char const* result = sha256_hash((char*)digestData);
			delete[] digestData;
			return result;
		}
		else if (strcmp(fAlgorithm, "MD5") == 0) {
			char ha1Buf[33];
			char entityhex[33];
			if (fPasswordIsMD5) {
				strncpy(ha1Buf, password(), 32);
				ha1Buf[32] = '\0'; // just in case
			}
			else {
				unsigned const ha1DataLen = strlen(username()) + 1
					+ strlen(realm()) + 1 + strlen(password());
				unsigned char* ha1Data = new unsigned char[ha1DataLen + 1];
				sprintf((char*)ha1Data, "%s:%s:%s", username(), realm(), password());
				our_MD5Data(ha1Data, ha1DataLen, ha1Buf);
				delete[] ha1Data;
			}

			unsigned ha2DataLen = 0;
			unsigned char* ha2Data = NULL;
			if (strcmp(fQop, "auth-int") == 0)
			{
				size_t n = sizeof(entityhex) / sizeof(entityhex[0]);
				ha2DataLen = strlen(cmd) + 1 + strlen(url) + 1 + n;
				ha2Data = new unsigned char[ha2DataLen + 1];
				sprintf((char*)ha2Data, "%s:%s:%s", cmd, url, entityhex);
			}
			else {
				ha2DataLen = strlen(cmd) + 1 + strlen(url);
				ha2Data = new unsigned char[ha2DataLen + 1];
				sprintf((char*)ha2Data, "%s:%s", cmd, url);
			}

			char ha2Buf[33];
			our_MD5Data(ha2Data, ha2DataLen, ha2Buf);
			delete[] ha2Data;

			unsigned digestDataLen = 0;
			unsigned char* digestData = NULL;
			if (strcmp(fQop, "auth") == 0) {
				digestDataLen = 32 + 1 + strlen(nonce()) + 1
					+ strlen(nc()) + 1 + strlen(cnonce()) + 1 + strlen(qop()) + 1 + 32;
				digestData = new unsigned char[digestDataLen + 1];
				sprintf((char*)digestData, "%s:%s:%s:%s:%s:%s",
					ha1Buf, nonce(),
					nc(), cnonce(), qop(), ha2Buf);
			}
			else {
				digestDataLen = 32 + 1 + strlen(nonce()) + 1 + 32;
				digestData = new unsigned char[digestDataLen + 1];
				sprintf((char*)digestData, "%s:%s:%s",
					ha1Buf, nonce(), ha2Buf);
			}

			char const* result = our_MD5Data(digestData, digestDataLen, NULL);
			delete[] digestData;
			return result;
		}
  } else {
  char ha1Buf[33];
  if (fPasswordIsMD5) {
    strncpy(ha1Buf, password(), 32);
    ha1Buf[32] = '\0'; // just in case
  } else {
    unsigned const ha1DataLen = strlen(username()) + 1
      + strlen(realm()) + 1 + strlen(password());
    unsigned char* ha1Data = new unsigned char[ha1DataLen+1];
    sprintf((char*)ha1Data, "%s:%s:%s", username(), realm(), password());
    our_MD5Data(ha1Data, ha1DataLen, ha1Buf);
    delete[] ha1Data;
  }

  unsigned const ha2DataLen = strlen(cmd) + 1 + strlen(url);
  unsigned char* ha2Data = new unsigned char[ha2DataLen+1];
  sprintf((char*)ha2Data, "%s:%s", cmd, url);
  char ha2Buf[33];
  our_MD5Data(ha2Data, ha2DataLen, ha2Buf);
  delete[] ha2Data;

  unsigned const digestDataLen
    = 32 + 1 + strlen(nonce()) + 1 + 32;
  unsigned char* digestData = new unsigned char[digestDataLen+1];
  sprintf((char*)digestData, "%s:%s:%s",
          ha1Buf, nonce(), ha2Buf);
  char const* result = our_MD5Data(digestData, digestDataLen, NULL);
  delete[] digestData;
  return result;
  }
}

void Authenticator::reclaimDigestResponse(char const* responseStr) const {
  delete[](char*)responseStr;
}

void Authenticator::resetRealmAndNonce() {
  delete[] fRealm; fRealm = NULL;
  delete[] fNonce; fNonce = NULL;
}

void Authenticator::resetUsernameAndPassword() {
  delete[] fUsername; fUsername = NULL;
  delete[] fPassword; fPassword = NULL;
  fPasswordIsMD5 = False;
}

void Authenticator::resetOpaqueAndAlgorithmAndQop() {
  delete[] fOpaque; fOpaque = NULL;
  delete[] fAlgorithm; fAlgorithm = NULL;
  delete[] fQop; fQop = NULL;
}

void Authenticator::resetCnonceAndNonceCount() {
	delete[] fCnonce; fCnonce = NULL;
	delete[] fNonceCount; fNonceCount = NULL;
}

void Authenticator::resetHashType() {
	fHashType = -1;
}

void Authenticator::assignRealmAndNonce(char const* realm, char const* nonce) {
  fRealm = strDup(realm);
  fNonce = strDup(nonce);
}

void Authenticator::assignUsernameAndPassword(char const* username, char const* password, Boolean passwordIsMD5) {
  if (username == NULL) username = "";
  if (password == NULL) password = "";

  fUsername = strDup(username);
  fPassword = strDup(password);
  fPasswordIsMD5 = passwordIsMD5;
}

void Authenticator::assignOpaqueAndAlgorithmAndQop(char const* opaque, char const* algorithm, char const* qop) {
  //resetOpaqueAndAlgorithmAndQop();
  fOpaque = strDup(opaque);
  fAlgorithm = strDup(algorithm);
  fQop = strDup(qop);
}

void Authenticator::assignCnonceAndNonceCount(char const* cnonce, char const* nc) {
  //resetCnonceAndNonceCount();
  fCnonce = strDup(cnonce);
  fNonceCount = strDup(nc);
}

void Authenticator::assignHashType(int hashType) {
	fHashType = hashType;
}

void Authenticator::assign(char const* realm, char const* nonce,
			   char const* username, char const* password, Boolean passwordIsMD5) {
  assignRealmAndNonce(realm, nonce);
  assignUsernameAndPassword(username, password, passwordIsMD5);
}

void Authenticator::assign(char const* realm, char const* nonce,
				 char const* username, char const* password, char const* opaque, char const* algorithm,
				 char const* qop, char const* cnonce, char const* nc, Boolean passwordIsMD5, int hashType) {
  assignRealmAndNonce(realm, nonce);
  assignOpaqueAndAlgorithmAndQop(opaque, algorithm, qop);
  assignUsernameAndPassword(username, password, passwordIsMD5);
  assignCnonceAndNonceCount(NULL, NULL);
	assignHashType(hashType);
}