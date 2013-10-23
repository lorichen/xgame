#ifndef _RC4_H_
#define _RC4_H_

#include "CryptType.h"

namespace xs{ namespace CryptPool{

ulong Encrypt_rc4(uchar newbuf[], uchar buffer[], ulong length,uchar * key_,ulong keylen);
ulong Decrypt_rc4(uchar newbuf[], uchar buffer[], ulong length,uchar * key_,ulong keylen);
ulong Set_rc4_key(uchar keybuf[], ulong keylen);

}} 

#endif