#ifndef __lzss_H__
#define __lzss_H__

extern void Decode(BYTE *Src,int size,BYTE *Dst,int *pSize);
extern void Encode(BYTE *Src,int size,BYTE *Dst,int *pSize);

extern void Decode2(BYTE *Src,BYTE *Dst,int *pSize);
extern void Encode2(BYTE *Src,int size,BYTE *Dst,int *pSize);

#endif