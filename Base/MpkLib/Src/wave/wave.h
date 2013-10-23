#ifndef __WAVE_H__
#define __WAVE_H__

//-----------------------------------------------------------------------------
// Functions

#include "../../Include/MpkPort.h"

int  CompressWave  (unsigned char * pbOutBuffer, int dwOutLength, short * pwInBuffer, int dwInLength, int nCmpType, int nChannels);
int  DecompressWave(unsigned char * pbOutBuffer, int dwOutLength, unsigned char * pbInBuffer, int dwInLength, int nChannels);

#endif // __WAVE_H__
