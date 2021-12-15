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
 * @file SHA256.cpp
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

#include "SHA256.hh"

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#define SHA_PASSWORD_LENGTH	64+1

const unsigned char SHA256_KEY[SHA256_DIGEST_LENGTH] = 
{
	0x42, 0x71, 0xb5, 0xe9, 0x39, 0x59, 0x92, 0xab, 
	0xd8, 0x12, 0x24, 0x52, 0x72, 0x80, 0x9b, 0xc1,
	0xe4, 0xef, 0x0f, 0x24, 0x2d, 0x4a, 0x5c, 0x76,
	0x98, 0xa8, 0xb0, 0xbf, 0xc6, 0xd5, 0x06, 0x14
};

static void sha256_transform(const char *in, char * out)
{
	int i = 0;

	for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		out[i] = in[i] + SHA256_KEY[i];
	}
}


/**
* @fn 		: sha256_hash_using_key
* @brief 		: 입력받은 값과 key 값을 사용하여 hash 생성
* @param 	: in : plain, hash : hashed string
* @return 	: 0 : fail, 1 : ok
* @see 		: 
* @date 		: 20170104
* @author 	: SJMok
*/
extern "C" int sha256_hash_using_key(const char* in, char* hash)
{
	if(NULL == in)
	{
		return 0;
	}

	util::SHA256 sha256;
	char transform_str[SHA_PASSWORD_LENGTH] = {0,};
	sha256_transform(in, transform_str);
	string out = sha256.hash(transform_str);
	if(out.size() == 0)
	{
		return 0;
	}

	int len = out.size() + 1;
#ifndef _WIN32
	snprintf(hash, len, "%s", out.c_str());
#endif
	
	return 1;
}


/**
* @fn 		: sha256_compare_using_key
* @brief 		: 첫번째 인자로 입력받은 값을 key 값을 이용하여 hash 생성하고 두번째 인자와 비교
* @param 	: in : plain, enc_in : hashed string
* @return 	: 0 : fail, 1 : ok
* @see 		: 
* @date 		: 20170104
* @author 	: SJMok
*/
extern "C" int sha256_compare_using_key(const char* in, const char* enc_in)
{
	if(NULL == in)
	{
		return 0;
	}

	util::SHA256 sha256;
	char transform_str[SHA_PASSWORD_LENGTH] = {0,};
	sha256_transform(in, transform_str);
	return sha256.compare(transform_str, enc_in);
}


extern "C" char const* sha256_hash(const char* in)
{
	if(NULL == in)
	{
		return NULL;
	}

	util::SHA256 sha256;
	string out = sha256.hash(in);
	if(out.size() == 0)
	{
		return NULL;
	}

	int len = out.size() + 1;
	char* ret = (char*)malloc(len);
	if(NULL == ret)
	{
		return NULL;
	}
#ifndef _WIN32
	snprintf(ret, len, "%s", out.c_str());
#else
	_snprintf(ret, len, "%s", out.c_str());
#endif
	return ret;
}


/**
 *
 * @param in 
 * @param enc_in 
 * @return 
 */
extern "C" int sha256_compare(const char* in, const char* enc_in)
{
	if(NULL == in)
	{
		return 0;
	}

	util::SHA256 sha256;
	return sha256.compare(in, enc_in);
}

extern "C" char* sha256_hash_digest(const char* input1, const char* input2, const char* input3)
{
	if(NULL == input1 || NULL == input2 || NULL == input3)
	{
		return NULL;
	}

	util::SHA256 sha256;
	string hash1 = sha256.hash(input1);
	if(hash1.size() == 0)
	{
		return NULL;
	}
	sha256.init();
	string hash2 = sha256.hash(input2);	
	if(hash2.size() == 0)
	{
		return NULL;
	}
	sha256.init();	
	string inputEnd = hash1+string(input3)+hash2;
	string Response = sha256.hash(inputEnd);
	if(Response.size() == 0)
	{
		return NULL;
	}	

	int len = Response.size() + 1;
	char* ret = (char*)malloc(len);
	if(NULL == ret)
	{
		return NULL;
	}
#ifndef _WIN32
	snprintf(ret, len, "%s", Response.c_str());
#endif
	return ret;	
}

namespace util
{
/**
 *
 */
SHA256::SHA256()
{
	init();
}


/**
 *
 */
SHA256::~SHA256()
{

}


/**
 *
 */
void SHA256::init()
{
	SHA256_Init(&sha256);
}


/**
 *
 * @param in_str 
 * @param strLen 
 */
void SHA256::update(const char *in_str, int strLen)
{
	SHA256_Update(&sha256, in_str, strLen);
}


/**
 *
 */
void SHA256::final()
{
	SHA256_Final(hashBuff, &sha256);
}


/**
 *
 * @param in 
 * @return 
 */
string SHA256::hash(const string& in)
{
	char out_str[SHA_PASSWORD_LENGTH] = {0, };

	update(in.c_str(), in.size());
	final();

	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(out_str + (i * 2), "%02x", hashBuff[i]);
	}

	out_str[64] = '\0';

	return out_str;
}


/**
 *
 * @param in 
 * @param enc_in 
 * @return 
 */
bool SHA256::compare(const string& in, const string& enc_in)
{
	init();
	string ret = hash(in);
	return ret == enc_in;
}

}//namespace device_sunapi
