#ifndef __SceneBlock_H__
#define __SceneBlock_H__

#define  MAX_TILE_WIDTH  3200
#define  MAX_TILE_HEIGHT  3200
#define  MAX_TILE_WIDTH_INBYTE  (MAX_TILE_WIDTH / 8)

class SceneBlock
{
public:
	uchar *m_pData;
	int m_MapWidth;
	int m_MapHeight;
	
	uchar *m_pPathData;

	int  __GetRowBytes(int nWidth);
	bool __ReadData(uchar *pData, int nX, int nY);
	void __WriteData(uchar *pData, int nX, int nY, bool flag);

	BOOL m_bOK;
public:
	SceneBlock();
	virtual ~SceneBlock();

	bool SetMapSize(int nWidth, int nHeight);

	bool IsBlock(xs::Point pt){return IsBlock(pt.x,pt.y);}
	bool IsBlock(int nX, int nY);
	void SetBlock(int nX, int nY, bool block);

	BOOL SaveToFile(LPCSTR szFileName);
	BOOL LoadFromFile(LPCSTR szFileName);

	void Save(xs::Stream *pDataStream);
	void Load(xs::Stream *pStream);

	void ClearPath(void);
	bool GetPathPoint(int nX, int nY);
	void SetPathPoint(int nX, int nY, bool block);

	BOOL IsOK(void);
	void SetOK(BOOL bFlag);

};
 
#endif

