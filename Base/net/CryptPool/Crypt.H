#ifndef _CRYPT_H
#define _CRYPT_H

#include "CryptType.h"

namespace xs{ namespace CryptPool{

	ulong RandomAlgorithmId();
	ulong Encrypt(ulong id, uchar* buf, ulong length,uchar * key,ulong keylen);
	ulong Decrypt(ulong id, uchar* buf, ulong length,uchar * key,ulong keylen);
	//ulong SetSessionKey(ulong id,uchar * key,ulong keylen);


}} // 

#endif