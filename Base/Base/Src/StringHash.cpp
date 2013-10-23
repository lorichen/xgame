/**
* Created by pk 2008.01.07
*/


#include "stdafx.h"
#include <stdlib.h>
#include "StringHash.h"

#define new RKT_NEW

namespace xs {

	static ulong gLower[256] = 
	{
		0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
		16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
		32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
		48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
		64,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,  91,  92,  93,  94,  95,
		96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
		128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
		144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
		160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
		176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
		192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
		208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
		224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
		240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
	};

	static ulong gHashBuffer[0x500];

	void initStringHashBuffer()
	{
		ulong seed   = 0x00100001;

		memset(gHashBuffer, 0, sizeof(gHashBuffer));

		for (uint index1 = 0; index1 < 0x100; index1++)
		{
			for(uint index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
			{
				ulong temp1, temp2;
				seed  = (seed * 125 + 3) % 0x2AAAAB;
				temp1 = (seed & 0xFFFF) << 0x10;

				seed  = (seed * 125 + 3) % 0x2AAAAB;
				temp2 = (seed & 0xFFFF);

				gHashBuffer[index2] = (temp1 | temp2);
			}
		}
	}



	// 性能：10个大写字符，调用一次耗时：0.000716毫秒
	// 环境：Release版,CPU Athon 1800+ ,Mem 1G
	RKT_API ulong hashString(const char* str, ulong type)
	{
		ulong seed1 = 0x7FED7FED;
		ulong seed2 = 0xEEEEEEEE;
		ulong ch;
		const uchar* ustr = (const uchar*)str;

		while (*ustr != 0)
		{
			ch = gLower[*ustr++];
			seed1 = gHashBuffer[(type<<8) + ch] ^ (seed1 + seed2);
			seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
		}

		return seed1;
	}

	RKT_API bool makeMap(uchar* buffer, ulong size, ulong key)
	{
		uint64 seed2 = 0xEEEEEEEEEEEEEEEEULL;
		uint64 seed1 = (key ^ 0xEEEEEEEEULL);
		seed1 <<= 32;
		seed1 |= ~(key ^ 0xEEEEEEEEULL);
		uint64* pos = (uint64*)buffer;
		ulong size8 = size >> 3;
		ulong size_trail = size & 7;
		uint64 ch;

		while (size8-- > 0)
		{
			seed2 += ((uint64*)gHashBuffer)[0x200 + (seed1 & 0x7F)];
			ch     = *pos ^ (seed1 + seed2);
			seed1  = ((~seed1 << 0x2A) + 0x1111111111111111ULL) | (seed1 >> 0x16);
			seed2  = /*ch + */seed2 + (seed2 << 0xA) + 3;
			*pos++ = ch;
		}

		uchar* pos_trail = (uchar*)pos;
		while (size_trail-- > 0)
			*pos_trail++ ^= ((uchar*)&seed1)[size_trail];

		return true;
	}

} // namespace
