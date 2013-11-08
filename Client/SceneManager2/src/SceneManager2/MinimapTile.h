#ifndef __MinimapTile_H__
#define __MinimapTile_H__

class MinimapEyeshot;

class MinimapTile
{
public:
	MinimapTile();
	~MinimapTile();

	void setResHandle(handle h)
	{
		m_handle = h;
	}
	handle getResHandle()
	{
		return m_handle;
	}
	ITexture* loadResource();
	ITexture* getTexture();

private:
	handle		m_handle;

public:
	xs::Point		m_ptCoord;	//在地图上的MinimapTile坐标
	MinimapEyeshot*	m_pParent;
};

#endif