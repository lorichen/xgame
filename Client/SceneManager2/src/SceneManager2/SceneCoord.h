#ifndef __SceneCoord_H__
#define __SceneCoord_H__

class SceneCoord
{
public:
	//added by xxh
	// 关于tile坐标和pixel坐标（世界坐标)的简述：
	// tile坐标如下：
	// /(y)	
	// \(x)
	// pixel坐标(世界坐标)如下：
	// -(x)
	// |(y)

	// tile坐标的点(0,183)对应pixel坐标的(0，0)点。而且有ptTile.x每加1，ptWorld.x和ptWorld.y都1相应单位
	// ptTile.y每加1，ptWorld.x加1单位，ptWorld.y减1单位。对pixel坐标，x方向的单位为32像素，y方向的单位为16像素，
	// 从而得到tile作保向pixel坐标转换公式：
	// pWorld.x = [ ( pTile.x-0) + (pTile.y - 183) ] * 32 + 0 => (pTile.x +pTile.y -m_yOff)<< 5 ;
	// pWorld.y = [ (pTile.x-0) - (pTile.y -183) ] * 16 + 0  => (pTile.x - pTile.y + m_yOff)<< 4;

	// 对于将world坐标映射为tile坐标，稍微有些复杂。首先肯定是pWorld.x/32和pWorld.y/16，得到近似的相对于(0,183)的相对坐标，
	// 此时要得到近似的tile坐标，只要这样计算就可以了：
	// pTile.x = (pWorld.x/64 +pWorld.y/32) /2 + 0 => pWorld.x>>6 + pWorld.y >>5;
	// pTile.y = ( (pWorld.x/32 - pWorld.y/16 ) + 2* 183 ) / 2  => pWorld.x>>6 - pWorld.y>>5 + m_myOff
	// 但是不一定会整除，留下的余数会调整tile值，不同的余数调整值不同，具体调整值看m_TilePoint的定义，这里也不好表述，看下图，5个不同区域，调整值不一样
	// 所以OffsetX和OffsetY有5个不同的对应值
	// ________
	// |  /\  |
	// | /  \ |
	// |/    \|
	// |\    /|
	// | \  / |
	// |__\/__|
	//
	// 所以得到公式：
	// pTile.x = pWorld.x>>6 + pWorld.y>>5 + OffSetX[off]
	// pTile.y = pWorld.x>>6 - pWorld.y>>5 + OffSetY[off] + m_myOff


	void tile2Pixel(IN const POINT& ptTile, OUT POINT& ptTileCenter) const
	{
		ptTileCenter.x = (ptTile.x + ptTile.y - m_yOff) << 5; // * 64 / 2;
		ptTileCenter.y = (ptTile.x - ptTile.y + m_yOff) << 4; // * 32 / 2;
	}
	void pixel2Tile(IN const POINT& ptWorld, OUT POINT& ptTile) const
	{
		const static int OffsetX[5] = { 0, 1, 1, 1, 2};
		const static int OffsetY[5] = { 0, 1, 0,-1, 0};

		int kx = ptWorld.x >> 6;
		int ky = ptWorld.y >> 5;
		int off = m_TilePoint[ptWorld.y & 0x1F][ptWorld.x & 0x3F];
		ptTile.x = kx + ky + OffsetX[off];
		ptTile.y = kx - ky + OffsetY[off] + m_yOff;
	}

	RECT pixelRectToAreaTileRect(RECT rc) const
	{
		IntersectRect(&rc, &m_rcMap, &rc);
#if 0
		rc.left = (Ceil(rc.left, 64)) * 64;
		rc.top = (Ceil(rc.top, 32)) * 32;
		rc.right &= ~63;
		rc.bottom &= ~31;
#else
		rc.left = (Ceil(rc.left, 64)) * 64;
		rc.top = (Ceil(rc.top, 32)) * 32;
		rc.right  = Ceil(rc.right,64) * 64;
		rc.bottom = Ceil(rc.bottom,32) * 32;
#endif
		RECT rcTileArea;
		pixel2Tile((const POINT&)rc, (POINT&)rcTileArea);
		rcTileArea.right = Ceil(rc.right - rc.left, 64);
		rcTileArea.bottom = Ceil(rc.bottom - rc.top, 32);
        //  待确认的实现代码：
		//	此处暂时不理解为什么要使用m_nMatrixHeight来进行判断，按理说被修改的是rcTileArea.right，是做为横坐标
		//	来使用的，此处似乎应该使用m_nMatrixWidth？
		//	此外，使用m_nMatrixHeight来进行判断，会导致地图的最后两列的tile在后继的EnumRect::EnumTileArea()计算操
		//	作中没有被计算在内，修改为使用m_nMatrixWidth后问题暂时没有再出现；
		//	由于对tile坐标的转换的相关代码还在进一步学习中，因此此处的修改所造成的影响需要进行进一步的观察；

		//added by xxh
		// rcTileArea.top是tile的x坐标，rcTileArea.right是tile在x方向的相对长度，而不是平常的绝度长度。
		//我想用m_nMatrixHeight来进行判断，是因为( m_nMatrixHeight =nRows + nCols)代表了在tile坐标中x坐标的最大容许值，听起来有点怪，
		//因为tile坐标是斜的,可以在纸上画画试试。
		//我不明白的是，这一句是什么意思：rcTileArea.bottom = rcTileArea.top + 1;为什么要修改。还好这一句一般不会执行
		//否则可能会出bug
#if 0
		if (rcTileArea.top + rcTileArea.right >= m_nMatrixHeight)
			rcTileArea.right = m_nMatrixHeight - rcTileArea.top - 1;
#endif
		if (rcTileArea.top + rcTileArea.right >= m_nMatrixWidth)
		{
			rcTileArea.right = m_nMatrixWidth - rcTileArea.top - 1;
		}

		if (rcTileArea.top < rcTileArea.bottom - 1)
			rcTileArea.bottom = rcTileArea.top + 1;

		return rcTileArea;
	}

	RECT _pixelRectToAreaTileRect(RECT rc) const
	{
		RECT rcTileArea;
		pixel2Tile((const POINT&)rc, (POINT&)rcTileArea);
		rcTileArea.right = Ceil(rc.right - rc.left, 64);
		rcTileArea.bottom = Ceil(rc.bottom - rc.top, 32);

		return rcTileArea;	
	}

	bool create(int nWidth, int nHeight)
	{
		int w = nWidth, h = nHeight;
		w &= ~63;
		h &= ~31;

		int nRows = Ceil(nHeight, 32);
		int nCols = Ceil(nWidth, 64);

		m_rcMap.right = nWidth;
		m_rcMap.bottom = nHeight;

		// note by zjp；
		m_yOff = nRows - 1;// （这个是意思是说地图的像素坐标(0,0)对应的tile坐标为(0,m_yOff)）这是因为在编辑器里面是这么设定的，将每一张地图的左上角都放在(0,nRows - 1)这个tile坐标处。
		// 下面这两个等式的意思是在tile坐标系中（45°菱形坐标系），地图沿tile坐标系方向上最大的tile值是分别多少。这两个值是怎么得出来的呢，可以参考场景编辑器，打开一幅地图自己算下，可以验证结果确实如此。
		// 这样我们就知道一幅地图对应的有效tile范围是:
		// 0<=tile.X<=m_nMatrixWidth
		// 0<=tile.Y<=m_nMatrixHeight
		// 注意，并不代表处在上述有效范围内的tile坐标就能对应到有效的地图坐标，比如tile坐标（0，0）就找不到相应的地图坐标，因此如果建立一个m_nMatrixWidth*m_nMatrixHeight大小的tile数组，其中有有一部分tile是没有用到的，这部分tile在寻路或
		// 其它用途的时候可以视为有阻挡的或无效的tile。
		m_nMatrixWidth = nRows + nCols;
		m_nMatrixHeight = m_nMatrixWidth - 1;
		return true;
	}
	int getMatrixWidth()
	{
		return m_nMatrixWidth;
	}
	int getMatrixHeight()
	{
		return m_nMatrixHeight;
	}
	RECT& getMapRect()
	{
		return m_rcMap;
	}
	SceneCoord()
	{
		m_yOff = 0;
		m_nMatrixWidth = m_nMatrixHeight = 0;
		memset(&m_rcMap, 0, sizeof(m_rcMap));
	}
	~SceneCoord()
	{
		
	}
	const static char m_TilePoint[32][64];
private:
	int		m_yOff;
	RECT	m_rcMap; // 整个地图的矩形信息
	int		m_nMatrixWidth;
	int		m_nMatrixHeight;
};

#endif
