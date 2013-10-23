/******************************************************************************
模块名：
	tea.h
描述：
	tea算法
作者：
	
******************************************************************************/

#ifndef _TEA_H_
#define _TEA_H_

#include "CryptType.h"

namespace xs{ namespace CryptPool{

ulong Encrypt_tea(uchar newbuf[], uchar buffer[], ulong length,uchar * key_,ulong keylen);
ulong Decrypt_tea(uchar newbuf[], uchar buffer[], ulong length,uchar * key_,ulong keylen);
ulong Set_tea_key(uchar keybuf[],ulong keylen);

}} //

#endif