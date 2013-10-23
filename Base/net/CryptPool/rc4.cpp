

#include "stdafx.h"
#include "rc4.h"

namespace xs{ namespace CryptPool{

enum{
	KEY_SIZE = 16
};

//static uint8 key[KEY_SIZE] = {
//	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
//};

//static uint8 key[KEY_SIZE];


//static uint8 S[256];

static void Swap(uint8 S[], int i, int j);
static void KSA(uint8 S[],uchar *key);
static ulong Crypt_rc4(uchar newbuf[], uchar buffer[], ulong length,uint8 key[]);

ulong Set_rc4_key(uchar keybuf[],ulong keylen)
{
	assert(KEY_SIZE==keylen);
	//memcpy_s(key,sizeof(uchar),keybuf,keylen);
	return 1;
}

static void Swap(uint8 a[], int i, int j)
{
	uint8 t;
	t = a[i];
	a[i] = a[j];
	a[j] = t;
}

static void KSA(uint8 S[],uchar *key)
{
	int i, j;

	for(i = 0; i < 256; i++)
		S[i] = i;
	for(i = 0, j = 0; i < 256; i++){
		j = (j+S[i]+key[i%KEY_SIZE]) % 256;
		Swap(S, i, j);
	}
}

static ulong Crypt_rc4(uchar newbuf[], uchar buffer[], ulong length,uint8 key[])
{
	ulong i;
	int x, y, t;

	//KSA();
	x = y = 0;
	for(i = 0; i < length; i++){
		x = (x+1) % 256;
		y = (y+key[x]) % 256;
		Swap(key, x, y);
		t = (key[x]+key[y]) % 256;
		newbuf[i] = buffer[i] ^ key[t];
	}

	return length;
}

ulong Encrypt_rc4(uchar newbuf[], uchar buffer[], ulong length,uchar * key_,ulong keylen)
{
	//memcpy_s(key,sizeof(char),key_,keysize);
	uint8 S[256];
	KSA(S,key_);
	return Crypt_rc4(newbuf, buffer, length,S);
}

ulong Decrypt_rc4(uchar newbuf[], uchar buffer[], ulong length,uchar * key_,ulong keylen)
{
	//memcpy_s(key,sizeof(char),key_,keysize);
	uint8 S[256];
	KSA(S,key_);
	return Crypt_rc4(newbuf, buffer, length,S);
}

}} //