/***********************************************************************************
 * Copyright(c) 2014 by Samsung Techwin, Inc.
 *
 * This software is copyrighted by, and is the sole property of Samsung Techwin.
 * All rights, title, ownership, or other interests in the software remain the
 * property of Samsung Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Samsung Techwin.
 *
 * Samsung Techwin reserves the right to modify this software without notice.
 *
 * Samsung Techwin, Inc.
 * KOREA
 * http://www.samsungtechwin.co.kr
 *********************************************************************************/

/**
 * @file SHA256.hpp
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

#ifndef __SHA256_HPP__
#define __SHA256_HPP__

#include <string>
#include <openssl/sha.h>

using namespace std;

extern "C" char const* sha256_hash(const char* in);

namespace util
{
/**
 *
 * @class 
 * @brief 
 */
class SHA256
{
public:
	SHA256();
	~SHA256();
	void init();	
	string hash(const string& in);
	bool compare(const string& in, const string& enc_in);
private:

	void update(const char *in_str, int strLen);
	void final();
private:
	SHA256_CTX sha256;
	unsigned char hashBuff[SHA256_DIGEST_LENGTH];
};

} //namespace device_sunapi

#endif // __SHA256_HPP__
