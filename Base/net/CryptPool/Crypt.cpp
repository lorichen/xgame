
#include "stdafx.h"
#include "Crypt.h"
#include "rc4.h"
#include "tea.h"
#include "blowfish.h"

namespace xs{ namespace CryptPool{

typedef ulong (*CryptProc_T)(uchar*, uchar*, ulong,uchar * ,ulong);
typedef ulong (*SetSessionKey_T)(uchar*, ulong);

static struct{
	CryptProc_T pfnEncrypt;
	CryptProc_T pfnDecrypt;
}g_stCryptPool[] = {
	{Encrypt_rc4, Decrypt_rc4},
	{Encrypt_tea, Decrypt_tea},
	//{Encrypt_blowfish, Decrypt_blowfish}
};//修改这里有修改服务端

static SetSessionKey_T g_stSetKeyPool[]=
{
	Set_rc4_key,
	Set_tea_key,
	//Set_blowfish_key,
};


static const ulong g_nPoolSize = sizeof(g_stCryptPool)/sizeof(g_stCryptPool[0]);

ulong RandomAlgorithmId()
{
    unsigned int t = (unsigned int)time(NULL);
    srand(t);
    return rand() % g_nPoolSize;
}




ulong Encrypt(ulong id, uchar* buf, ulong length,uchar * key,ulong keylen)
{
	assert(id < g_nPoolSize);
	return g_stCryptPool[id].pfnEncrypt(buf, buf, length,key,keylen);
}

ulong Decrypt(ulong id, uchar* buf, ulong length,uchar * key,ulong keylen)
{
	assert(id < g_nPoolSize);
	return g_stCryptPool[id].pfnDecrypt(buf, buf, length,key,keylen);
}

}} //