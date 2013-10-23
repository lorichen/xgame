#ifndef _BLOWFISH_H_
#define _BLOWFISH_H_

#include "CryptType.h"

namespace xs{ namespace CryptPool{

ulong Encrypt_blowfish(uchar newbuf[], uchar buffer[], ulong length);
ulong Decrypt_blowfish(uchar newbuf[], uchar buffer[], ulong length);
ulong Set_blowfish_key(uchar keybuf[],ulong keylen);

}} // 
#endif