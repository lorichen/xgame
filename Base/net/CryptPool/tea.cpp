
#include "stdafx.h"
#include "tea.h"
#include <assert.h>

namespace xs{ namespace CryptPool{

enum{
	KEY_SIZE = 16
};

//static uint32 key[KEY_SIZE] ;


//static uint32 key[KEY_SIZE] = {
//	0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210,
//	0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476,
//	0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
//	0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
//};


// tea的分组长度为64位，密钥长度为128位
static void Encrypt(uint32 *dst, uint32 *src, uint32 *key);
static void Decrypt(uint32 *dst, uint32 *src, uint32 *key);


ulong Set_tea_key(uchar keybuf[],ulong keylen)
{
	assert(KEY_SIZE*4==keylen);
	//memcpy_s(key,sizeof(uchar),keybuf,keylen);
	return 1;
}

static void Encrypt(uint32 *dst, uint32 *src, uint32 *key)
{
	uint32 sum = 0;
	uint32 y = src[0], z = src[1];
	uint32 delta = 0x9e3779b9;	// 密钥调度常数
	int i = 0;

	while(i++ < 32){
		sum += delta;
		y += ((z<<4)+key[0]) ^ (z+sum) ^ ((z>>5)+key[1]);
		z += ((y<<4)+key[2]) ^ (y+sum) ^ ((y>>5)+key[3]);
	}
	dst[0] = y;
	dst[1] = z;
}

static void Decrypt(uint32 *dst, uint32 *src, uint32 *key)
{
	uint32 sum;
	uint32 y = src[0], z = src[1];
	uint32 delta = 0x9e3779b9;	// 密钥调度常数
	int i = 0;

	sum = delta << 5;
	while(i++ < 32){
		z -= ((y<<4)+key[2]) ^ (y+sum) ^ ((y>>5)+key[3]);
		y -= ((z<<4)+key[0]) ^ (z+sum) ^ ((z>>5)+key[1]);
		sum -= delta;
	}

	dst[0] = y;
	dst[1] = z;
}

// 分组长度为16位的tea加密算法
static void Encrypt16(uint8 *dst, uint8 *src, uint8 *key)
{
	uint8 sum = 0;
	uint8 y = src[0], z = src[1];
	uint8 delta = 0xb9;
	int i = 0;

	while(i++ < 32){
		sum += delta;
		y += ((z<<4)+key[0]) ^ (z+sum) ^ ((z>>5)+key[1]);
		z += ((y<<4)+key[2]) ^ (y+sum) ^ ((y>>5)+key[3]);
	}

	dst[0] = y;
	dst[1] = z;
}

// 分组长度为16位的tea解密算法
static void Decrypt16(uint8 *dst, uint8 *src, uint8 *key)
{
	uint8 sum;
	uint8 y = src[0], z = src[1];
	uint8 delta = 0xb9;	// 密钥调度常数
	int i = 0;

	sum = delta << 5;
	while(i++ < 32){
		z -= ((y<<4)+key[2]) ^ (y+sum) ^ ((y>>5)+key[3]);
		y -= ((z<<4)+key[0]) ^ (z+sum) ^ ((z>>5)+key[1]);
		sum -= delta;
	}

	dst[0] = y;
	dst[1] = z;
}



ulong Encrypt_tea_(uchar newbuf[], uchar buffer[], ulong length,uint32 * key_,ulong keylen)
{
	ulong n, m, i;
	uint32 *psrc, *pdst, *pend;
	uint8 *psrc8, *pdst8, *pend8, *pkey8;

	assert(newbuf && buffer && length > 0);

	n = length % 8;
	m = n % 2;

	pkey8 = (uint8 *)key_;
	psrc8 = (uint8 *)&buffer[m];
	pdst8 = (uint8 *)&newbuf[m];
	pend8 = (uint8 *)(buffer + n);
	if(m == 1)
		newbuf[0] = buffer[0] ^ pkey8[0];
	while(psrc8 < pend8){
		Encrypt16(pdst8, psrc8, pkey8);
		pdst8 += 2;
		psrc8 += 2;
		pkey8 += 4;
	}

	i = 0;
	psrc = (uint32 *)&buffer[n];
	pdst = (uint32 *)&newbuf[n];
	pend = (uint32 *)(buffer + length);
	while(psrc < pend){
		if(i == KEY_SIZE)
			i = 0;
		Encrypt(pdst, psrc, &key_[i]);
		pdst += 2;
		psrc += 2;
		i += 4;
	}

	return length;
}

ulong Encrypt_tea(uchar newbuf[], uchar buffer[], ulong length,uchar * key_,ulong keylen)
{
	return Encrypt_tea_(newbuf, buffer, length,(uint32 *) key_,keylen);
}

ulong Decrypt_tea_(uchar newbuf[], uchar buffer[], ulong length,uint32 * key_,ulong keylen)
{
	ulong n, m, i;
	uint32 *psrc, *pdst, *pend;
	uint8 *psrc8, *pdst8, *pend8, *pkey8;

	assert(newbuf && buffer && length > 0);

	n = length % 8;
	m = n % 2;

	pkey8 = (uint8 *)key_;
	psrc8 = (uint8 *)&buffer[m];
	pdst8 = (uint8 *)&newbuf[m];
	pend8 = (uint8 *)(buffer + n);
	if(m == 1)
		newbuf[0] = buffer[0] ^ pkey8[0];
	while(psrc8 < pend8){
		Decrypt16(pdst8, psrc8, pkey8);
		pdst8 += 2;
		psrc8 += 2;
		pkey8 += 4;
	}

	i = 0;
	psrc = (uint32 *)&buffer[n];
	pdst = (uint32 *)&newbuf[n];
	pend = (uint32 *)(buffer + length);
	while(psrc < pend){
		if(i == KEY_SIZE)
			i = 0;
		Decrypt(pdst, psrc, &key_[i]);
		pdst += 2;
		psrc += 2;
		i += 4;
	}

	return length;
}

ulong Decrypt_tea(uchar newbuf[], uchar buffer[], ulong length,uchar * key_,ulong keylen)
{
	return Decrypt_tea_(newbuf, buffer, length,(uint32 *) key_,keylen);
}

}} 