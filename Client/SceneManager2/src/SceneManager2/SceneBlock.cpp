// MapBlockBuffer.cpp: implementation of the SceneBlock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneBlock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

uchar BITMASK[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

/**
当地图宽度和高度都是65536时候，至少要3072 * 3071的Buffer
*/
SceneBlock::SceneBlock()
{
	m_pData = new uchar[MAX_TILE_WIDTH_INBYTE * MAX_TILE_HEIGHT];
	m_pPathData = new uchar[MAX_TILE_WIDTH_INBYTE * MAX_TILE_HEIGHT];
	memset(m_pData, 0, MAX_TILE_WIDTH_INBYTE * MAX_TILE_HEIGHT);
	if (m_pData && m_pPathData)
	{		
		m_MapWidth = MAX_TILE_WIDTH;
		m_MapHeight = MAX_TILE_HEIGHT;
	}	
	else
	{
		m_MapWidth = 0;
		m_MapHeight = 0;
	}
	m_bOK = FALSE;
}

SceneBlock::~SceneBlock()
{
	safeDeleteArray(m_pData);
	safeDeleteArray(m_pPathData);
}

bool SceneBlock::SetMapSize(int nWidth, int nHeight)
{
	if (nWidth > MAX_TILE_WIDTH || nHeight > MAX_TILE_HEIGHT
		|| m_pData == NULL)
	{
		m_bOK = FALSE;
		return false;
	}

	m_MapWidth = nWidth;
	m_MapHeight = nHeight;
	
	m_bOK = TRUE;
	return true;
}

bool SceneBlock::IsBlock(int nX, int nY)
{
	return __ReadData(m_pData, nX, nY);
}

void SceneBlock::SetBlock(int nX, int nY, bool block)
{
	__WriteData(m_pData, nX, nY, block);
}

void SceneBlock::Save(xs::Stream *pDataStream)
{
	pDataStream->write(&m_MapWidth,sizeof(m_MapWidth));
	pDataStream->write(&m_MapHeight,sizeof(m_MapHeight));

	int nRowBytes =__GetRowBytes(m_MapWidth);
	int nTotal = nRowBytes * m_MapHeight;
	uchar key = nTotal | nRowBytes;
	for (int i = 0; i < nTotal; ++i)
	{
		m_pData[i] = m_pData[i] ^ key;
	}

	pDataStream->write(m_pData,nRowBytes * m_MapHeight);

	for (int i = 0; i < nTotal; ++i)
	{
		m_pData[i] = m_pData[i] ^ key;
	}
}

void SceneBlock::Load(xs::Stream *pStream)
{
	pStream->read(&m_MapWidth, sizeof(m_MapWidth));
	pStream->read(&m_MapHeight, sizeof(m_MapHeight));

	int nRowBytes = __GetRowBytes(m_MapWidth);
	int nTotal  = nRowBytes * m_MapHeight;

	m_bOK = pStream->read(m_pData, nTotal);

	uchar key = nTotal | nRowBytes;
	for (int i = 0; i < nTotal; ++i)
	{
		m_pData[i] = m_pData[i] ^ key;
	}
}

BOOL SceneBlock::SaveToFile(LPCSTR szFileName)
{
	if (szFileName == NULL)
	{
		return FALSE;
	}

	FILE *fp = fopen(szFileName, "wb+");
	if (fp == NULL)
	{
		return FALSE;
	}

	fwrite(&m_MapWidth, sizeof(int), 1, fp);
	fwrite(&m_MapHeight, sizeof(int), 1, fp);
	
	int nRowBytes =__GetRowBytes(m_MapWidth);
	int nTotal = nRowBytes * m_MapHeight;
	uchar key = nTotal | nRowBytes;
	for (int i = 0; i < nTotal; ++i)
	{
		m_pData[i] = m_pData[i] ^ key;
	}
	fwrite(m_pData, nRowBytes, m_MapHeight, fp);
	fclose(fp);

	for (int i = 0; i < nTotal; ++i)
	{
		m_pData[i] = m_pData[i] ^ key;
	}
	
	return TRUE;
}

BOOL SceneBlock::LoadFromFile(LPCSTR szFileName)
{
	if (szFileName == NULL)
	{
		return FALSE;
	}
	
	FILE *fp = fopen(szFileName, "rb");
	if (fp == NULL)
	{
		m_bOK = FALSE;
		return FALSE;
	}
	
	fread(&m_MapWidth, sizeof(int), 1, fp);
	fread(&m_MapHeight, sizeof(int), 1, fp);     

	int nRowBytes = __GetRowBytes(m_MapWidth);
	int nActualRead = fread(m_pData, nRowBytes , m_MapHeight, fp);
	m_bOK = nActualRead == m_MapHeight;
	fclose(fp);	

	int nTotal  = nRowBytes * m_MapHeight;
	uchar key = nTotal | nRowBytes;
	for (int i = 0; i < nTotal; ++i)
	{
		m_pData[i] = m_pData[i] ^ key;
	}

	return TRUE;
}

static uchar Assign(uchar nOriginal, uchar nValue, BOOL bHigh)
{
	if (bHigh)
	{
		nValue = (nValue << 4) | 0x0f;
		nOriginal = nOriginal | 0xf0;
		return nOriginal & nValue;
	}

	nValue = nValue & 0x0f;
	nOriginal = nOriginal & 0xf0;
	return nOriginal | nValue;
}

void SceneBlock::ClearPath(void)
{
	memset(m_pPathData, 0, MAX_TILE_WIDTH_INBYTE * MAX_TILE_HEIGHT);
}

bool SceneBlock::GetPathPoint(int nX, int nY)
{	
	return __ReadData(m_pPathData, nX, nY);
}

void SceneBlock::SetPathPoint(int nX, int nY, bool block)
{	
	__WriteData(m_pPathData, nX, nY, block);
}

bool SceneBlock::__ReadData(uchar *pData, int nX, int nY)
{
	if (nX < 0 || nX > m_MapWidth || nY < 0 || nY > m_MapHeight)
	{		
		return false;
	}
	
	int nBytePos = nY * __GetRowBytes(m_MapWidth) + nX / 8;
	int nBitPos = nX % 8;	
	return (pData[nBytePos] & BITMASK[nBitPos]) != 0 ? true: false;
}

void SceneBlock::__WriteData(uchar *pData, int nX, int nY, bool flag)
{
	if (nX < 0 || nX > m_MapWidth || nY < 0 || nY > m_MapHeight)
	{
		//超出范围的,统一设为block
		return;
	}
	
	
	int nBytePos = nY * __GetRowBytes(m_MapWidth) + nX / 8;
	int nBitPos = nX % 8;
	if (flag)
	{
		pData[nBytePos] = pData[nBytePos] | BITMASK[nBitPos];
	}
	else
	{
		pData[nBytePos] = pData[nBytePos] & ~BITMASK[nBitPos];
	}
}

int  SceneBlock::__GetRowBytes(int nWidth)
{
	return nWidth / 8 + 1;
}

BOOL SceneBlock::IsOK(void)
{
	return m_bOK;
}

void SceneBlock::SetOK(BOOL bFlag)
{
	m_bOK = bFlag;
}