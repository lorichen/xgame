

#include "StdAfx.h"
#include "PixTextureFont.h"
#include "GDITextureFont.h"
#undef new

#define TEXTURE_SIZE	512.0f

//void * g_testITexture;
namespace xs
{


	
	//升高值
	float PixTextureFont::Ascender()
	{
		return 0;//m_pFontFamily->GetCellAscent(FontStyleRegular);
	}
	//降低值
	float PixTextureFont::Descender()
	{
		return 0;//m_pFontFamily->GetCellDescent(FontStyleRegular);
	}
	//行距
	float PixTextureFont::LineHeight()
	{
		return m_ui32Size;//m_pFontFamily->GetLineSpacing(FontStyleRegular);
	}

	//字符串的包围盒
	void  PixTextureFont::BBox( const wchar_t* wc, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)
	{
		float fHeight = 0;
		float fWidth = 0;
	
		while( *wc)
		{
			CharTextureInfo * pInfo = GetCharTextureInfo(*wc);
			if(pInfo==NULL)
				return;
			
			fWidth += pInfo->width;

			if(fHeight<pInfo->height)
				fHeight = pInfo->height;

			++wc;
		}
		llx = -fWidth/2;
		lly = -fHeight/2;
		llz = 0;
		urx = fWidth/2;
		ury = fHeight/2;
		urz = 0;
	}

	float PixTextureFont::Advance( const wchar_t* string)
	{
		return 0.0f;
	}
	
	
	//渲染一个字符号串
	void  PixTextureFont::Render(const wchar_t* c )
	{
		//return;
		PP_BY_NAME("PixTextureFont::Render");
		//SceneBlendFactor src,dst;
		//m_pRenderSystem->getSceneBlending(src,dst);
		//m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);

		//m_pRenderSystem->switchTo2D();
		//float fCurtop = 0;
		float fCurleft = 0;
		//看看矩阵都有没有缩放
		//Matrix4 mtxp = m_pRenderSystem->getProjectionMatrix();
		//Matrix4 mtxWiew = m_pRenderSystem->getViewMatrix();
		//Matrix4 mtxWorldSave = m_pRenderSystem->getWorldMatrix();
		//Matrix4 mtxWorld;
		
		while( *c)
		{
			CharTextureInfo * pInfo = GetCharTextureInfo(*c);
			
		
			if(pInfo==NULL)
				continue;
			CharRenderInfo renderinfo;
			renderinfo.pInfo = pInfo;
			renderinfo.left = m_pRenderSystem->getWorldMatrix().getTrans().x+fCurleft;
			renderinfo.top = m_pRenderSystem->getWorldMatrix().getTrans().y;
			renderinfo.color = m_pRenderSystem->getColor();
			renderinfo.scale = 1;
			vCharInfoForTex[pInfo->nTexIndex].push_back(renderinfo);

			/*
			PP_BY_NAME_START("PixTextureFont::RenderOneWord");
			m_pRenderSystem->setTexture(0,vTextures[pInfo->nTexIndex]);
			
			//mtxWorld.makeTrans(fCurleft,  fCurtop, 0.0f);
			//m_pRenderSystem->setWorldMatrix(mtxWorldSave * mtxWorld);

			m_pRenderSystem->beginPrimitive(PT_TRIANGLES);
			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[0].X, pInfo->uv[0].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft, fCurtop)); //Vector2(0, 0));//

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[0].X, pInfo->uv[1].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft, fCurtop+pInfo->height ));//Vector2(0, pInfo->height ));//

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[1].X, pInfo->uv[1].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft+pInfo->width, fCurtop+pInfo->height ));//Vector2(pInfo->width, pInfo->height ));//

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[1].X, pInfo->uv[1].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft+pInfo->width, fCurtop+pInfo->height ));

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[1].X, pInfo->uv[0].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft+pInfo->width, fCurtop));//Vector2(pInfo->width, 0));//

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[0].X, pInfo->uv[0].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft, fCurtop));

			m_pRenderSystem->endPrimitive();*/
			/*
			m_pRenderSystem->beginPrimitive(PT_QUADS);
			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[0].X, pInfo->uv[0].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft, fCurtop)); //Vector2(0, 0));//

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[0].X, pInfo->uv[1].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft, fCurtop+pInfo->height ));//Vector2(0, pInfo->height ));//

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[1].X, pInfo->uv[1].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft+pInfo->width, fCurtop+pInfo->height ));//Vector2(pInfo->width, pInfo->height ));//

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[1].X, pInfo->uv[0].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft+pInfo->width, fCurtop));//Vector2(pInfo->width, 0));//


			m_pRenderSystem->endPrimitive();

			//m_pRenderSystem->drawPrimitive(PT_TRIANGLES,v[i].ui32VertexStart,v[i].ui32NumVertices);*/
			
			/*
			float * pVB = (float*)m_pVB->lock(0,0,BL_DISCARD);
			float * pTexVB = (float*)m_pTexVB->lock(0,0,BL_DISCARD);

			pVB[0] = fCurleft;
			pVB[1] = fCurtop;
			pVB[2] = 0;
			pTexVB[0] = pInfo->uv[0].X;
			pTexVB[1] = pInfo->uv[0].Y;
			
	
			pVB[3] = fCurleft;
			pVB[4] = fCurtop+pInfo->height;
			pVB[5] = 0;
			pTexVB[2] = pInfo->uv[0].X;
			pTexVB[3] = pInfo->uv[1].Y;


			pVB[6] = fCurleft+pInfo->width;
			pVB[7] = fCurtop;
			pVB[8] = 0;
			pTexVB[4] = pInfo->uv[1].X;
			pTexVB[5] = pInfo->uv[0].Y;


			pVB[9] = fCurleft;
			pVB[10] = fCurtop+pInfo->height;
			pVB[11] = 0;
			pTexVB[6] = pInfo->uv[0].X;
			pTexVB[7] = pInfo->uv[1].Y;


			pVB[12] = fCurleft+pInfo->width;
			pVB[13] = fCurtop;
			pVB[14] = 0;
			pTexVB[8] = pInfo->uv[1].X;
			pTexVB[9] = pInfo->uv[0].Y;

			pVB[15] = fCurleft+pInfo->width;
			pVB[16] = fCurtop+pInfo->height;
			pVB[17] = 0;
			pTexVB[10] = pInfo->uv[1].X;
			pTexVB[11] = pInfo->uv[1].Y;

			m_pVB->unlock();
			m_pTexVB->unlock();

			m_pRenderSystem->setVertexVertexBuffer(m_pVB);
			m_pRenderSystem->setTexcoordVertexBuffer(0,m_pTexVB);

			m_pRenderSystem->drawPrimitive(PT_TRIANGLES, 0, 6);*/

			//PP_BY_NAME_STOP();
			
			fCurleft += pInfo->width;//-m_ui32Size/10;//宽度稍微做了下修正,让字和字挨紧点,一个字符串前面还有些空白
			//fCurtop += pInfo->height;

			++c;
		}
		
		//m_pRenderSystem->setWorldMatrix(mtxWorldSave);

		//m_pRenderSystem->setSceneBlending(src,dst);
		//m_pRenderSystem->setTexture(0,0);
		//m_pRenderSystem->switchTo2D();
	}
	//字符串的包围盒
	void  PixTextureFont::BBox( const char* str, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)
	{
		xs::autostring wString(str);
		BBox(wString.c_wstr(),llx, lly, llz, urx, ury, urz);
	}
	float PixTextureFont::Advance( const char* string)
	{
		return 0.0f;
	}
	//渲染一个字符号串
	void  PixTextureFont::Render(const char* string )
	{
		Render(xs::autostring::to_wchar(string));
	}

	PixTextureFont::PixTextureFont():m_strFontName(),
		m_ui32Size(0),
		m_bOutline(true),
		m_hdc(0),
		m_hFont(0),
		fontcolor(255,255,255),
		effectcolor(32,32,32),
		m_nDrawTextleft(0),
		m_nDrawTextTop(0),
		m_nBlankPixX(0),
		m_nBlankPixY(0),
		m_pVB(0),
		m_pTexVB(0)
	{
	}

	bool PixTextureFont::create(IRenderSystem *pRenderSystem,const wchar_t* szFontName,uint size)
	{
		if( 0 == pRenderSystem )
			return false;

		if( 0 == pRenderSystem->getTextureManager())
			return false;

		std::wstring fontName  = szFontName;
		if( fontName.empty() )
			return false;


		m_pRenderSystem =  pRenderSystem;
		m_pTextureManager =  m_pRenderSystem->getTextureManager();
		m_ui32Size = size;
		m_strFontName = szFontName;

		CharInfoForTex ci;
		vCharInfoForTex.push_back(ci);

		ITexture* pTex = m_pTextureManager->createEmptyTextureNoWait(TEXTURE_SIZE,TEXTURE_SIZE,PF_A8R8G8B8,FO_POINT,FO_POINT,FO_NONE);
		if( 0 == pTex )
			return false;
		vTextures.push_back( pTex);
		//g_testITexture = pTex;

		m_hdc = ::GetWindowDC(NULL);
		m_hFont = ::CreateFont(m_ui32Size,0,0,0,0,0,0,0,DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");   
		HFONT hOldFont = (HFONT)::SelectObject(m_hdc,m_hFont);
		GetTextMetrics(m_hdc,&m_textmetric);
		(HFONT)::SelectObject(m_hdc,hOldFont);

		wchar_t wc = L'弓';
		GetCharTextureInfo(wc);
		wc = 'f';
		GetCharTextureInfo(wc);
		wc = L'中';
		//for(int i=0;i<2000;i++)
		//{
			GetCharTextureInfo(wc);
		//	wc++;
		//}

		return true;
	}

	//PixTextureFont::PixTextureFont(IRenderSystem *pRenderSystem,ITextureManager *pTextureManager,const wchar_t * szFontName,uint size)
	//{
	//	m_pRenderSystem =  pRenderSystem;
	//	m_pTextureManager =  pTextureManager;

	//	//m_pFontFamily = new FontFamily( xs::autostring::to_wchar(szFontName));
	//	m_ui32Size = size;

	//	
	//	m_nDrawTextleft = 0;
	//	m_nDrawTextTop = 0;
	//
	//	vTextures.push_back( m_pTextureManager->createEmptyTexture(TEXTURE_SIZE,TEXTURE_SIZE,PF_A8R8G8B8,FO_POINT,FO_POINT,FO_NONE) );
	//	CharInfoForTex ci;
	//	vCharInfoForTex.push_back(ci);
	//
	//	m_hdc = ::GetWindowDC(NULL);
	//	m_hFont = ::CreateFont(m_ui32Size,0,0,0,0,0,0,0,DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");   
	//	HFONT hOldFont = (HFONT)::SelectObject(m_hdc,m_hFont);
	//	GetTextMetrics(m_hdc,&m_textmetric);
	//	(HFONT)::SelectObject(m_hdc,hOldFont);

	//	
	//	fontcolor = Color(255,255,255);
	//	
	//	//
	//	effectcolor = Color(32,32,32);
	//	m_bOutline = true;

	//	wchar_t wc = L'弓';
	//	GetCharTextureInfo(wc);
	//	wc = 'f';
	//	GetCharTextureInfo(wc);
	//	wc = L'中';
	//	GetCharTextureInfo(wc);
	//}

	void PixTextureFont::release()
	{
		STLReleaseSequence(vTextures);
		vCharInfoForTex.clear();
		safeRelease(m_pVB);
		safeRelease(m_pTexVB);
		delete this;
	}

	//PixTextureFont::~PixTextureFont()
	//{
	//	vTextures.clear();
	//	vCharInfoForTex.clear();
	//}

	FontRenderDirection PixTextureFont::setRenderDirection( FontRenderDirection rd)
	{
		return rd;
	}
	float PixTextureFont::setFontGap( float gap)
	{
		return 0.0f;
	}
	
	//  获取渲染时用到的纹理信息
	CharTextureInfo * PixTextureFont::GetCharTextureInfo(wchar_t wc)
	{
		PP_BY_NAME("PixTextureFont::GetCharTextureInfo");
		FontTexture::iterator it = m_FontTexture.find(wc);
		if(it != m_FontTexture.end())
			return &(*it).second;

		//看看这个字符需要多大区域
		static CharTextureInfo info;

		float fWidth=0.0f;
		float fHeight=0.0f;
	
		//测量出本字的宽度和高度
		HFONT hOldFont = (HFONT)::SelectObject(m_hdc,m_hFont);

		//MAT2 m2;
		GLYPHMETRICS gm;
		MAT2 m2 = {
			{0, 1},
			{0, 0},
			{0, 0},
			{0, 1},
		};
		DWORD dwSize = ::GetGlyphOutlineW(m_hdc,wc, GGO_BITMAP , &gm, 0, NULL, &m2);
		if(dwSize==-1||dwSize==GDI_ERROR)
			return NULL;
		//加2是为了行与行之间留点缝隙

		//相对于原点的偏移
		int offsetx = gm.gmptGlyphOrigin.x;
		int offsety = m_textmetric.tmHeight-m_textmetric.tmDescent-gm.gmptGlyphOrigin.y;

		int nNewWidth = gm.gmBlackBoxX+offsetx;
		
		int nNewHeight = gm.gmBlackBoxY+offsety;
		if(nNewHeight<m_ui32Size)
		{
			nNewHeight = m_ui32Size;
			nNewWidth +=1;
		}

		if(m_bOutline)
		{
			fWidth = nNewWidth+2;
			fHeight = nNewHeight+2;
		}
		else
		{
			fWidth = nNewWidth;
			fHeight = nNewHeight;
		}
		
	
		if( m_nDrawTextleft + fWidth >TEXTURE_SIZE-3 )
		{
			//换行
			//if( m_nDrawTextTop+fHeight>TEXTURE_SIZE-2 ) 空间不足够会导致上行文字的的图象被下行覆盖
			float fMaxHeight = LineHeight()>fHeight?LineHeight():fHeight;

			if( m_nDrawTextTop+LineHeight()>TEXTURE_SIZE-3 )
			{
				//本纹理已经容不下字了,创建一张新的
				vTextures.push_back( m_pTextureManager->createEmptyTextureNoWait(TEXTURE_SIZE,TEXTURE_SIZE,PF_A8R8G8B8,FO_POINT,FO_POINT,FO_NONE) );

				//g_testITexture = vTextures[vTextures.size()-1];

				CharInfoForTex ci;
				vCharInfoForTex.push_back(ci);

				m_nDrawTextleft = 0;
				m_nDrawTextTop = 0;
				info.left = 0;
				info.top = 0;
			}
			else
			{
				m_nDrawTextleft = fWidth+3;

				//
				//m_nDrawTextTop = m_nDrawTextTop+fHeight+3;//空间不足够会导致上行文字的的图象被下行覆盖
				m_nDrawTextTop = m_nDrawTextTop+LineHeight()+3;
				info.left = 0;
				info.top = m_nDrawTextTop;
			}
		}
		else
		{
			info.left = m_nDrawTextleft;
			info.top = m_nDrawTextTop;
			m_nDrawTextleft = m_nDrawTextleft+fWidth+3;
		}
		
		info.width = fWidth;
		info.height = fHeight;
		info.nTexIndex = vTextures.size() -1;

		
		//将本字符的图形放到对应的区域
		if(m_bOutline)
			effectGlyphToBuffer(&info, wc,dwSize);
		else
			drawGlyphToBuffer(&info, wc,dwSize);

		info.uv[0].X =  static_cast<float>(info.left) / TEXTURE_SIZE;
		info.uv[0].Y =  static_cast<float>(info.top) / TEXTURE_SIZE;
		info.uv[1].X =  static_cast<float>(info.left+info.width) / TEXTURE_SIZE;
		info.uv[1].Y =  static_cast<float>(info.top +info.height) / TEXTURE_SIZE;

		m_FontTexture[wc] = info;
		
		::SelectObject(m_hdc,hOldFont);
		return &info;
	}

	//小字体获取字的点阵数据
	//小字体必须获取获取点阵数据,小字矢量缩小后不清晰,有灰度
	void PixTextureFont::drawGlyphToBuffer(CharTextureInfo * pInfo,wchar_t szText,DWORD dwSize)
	{
		GLYPHMETRICS gm;
		MAT2 m2 = {
			{0, 1},
			{0, 0},
			{0, 0},
			{0, 1},
		};
		BYTE * pBuf = new BYTE[dwSize];//点阵数
		::GetGlyphOutlineW( m_hdc,szText, GGO_BITMAP, &gm, dwSize, pBuf, &m2);
		DWORD dwErr = GetLastError();
		if(dwSize==-1||dwSize==GDI_ERROR)
			return;

		int nWidth = gm.gmBlackBoxX;
		int nHeight = gm.gmBlackBoxY;

		//相对于原点的偏移
		int offsetx = gm.gmptGlyphOrigin.x;
		int offsety = m_textmetric.tmHeight-m_textmetric.tmDescent-gm.gmptGlyphOrigin.y;

		int nNewWidth = nWidth+offsetx;
		int nNewHeight = nHeight+offsety;
		if(nNewHeight<m_ui32Size)
		{
			nNewHeight = m_ui32Size;
			nNewWidth +=1;
		}

		uint *pData = new uint[nNewWidth * nNewHeight];//纹理数据
		memset(pData,0,sizeof(uint) * nNewWidth * nNewHeight);

		int nStride = dwSize / nHeight;

		for(int y=0;y<nHeight;++y)//行 
		{
			for(int x=0;x<nStride;++x)//列
			{
				for(int k=0;k<8;++k)//每个字节 
				{
					if((pBuf[y*nStride+x]>>(7-k))&1) 
					{
						int nXPix = x*8+k;//本行的第几个象素
						if(nXPix<nWidth)
						{
							pData[nNewWidth*(y+offsety)+nXPix+offsetx] = fontcolor.GetValue();
							//::OutputDebugStringW(L"1");
						}
					}
					else
					{
						//::OutputDebugStringW(L"0");
					}
				}
			}
			//::OutputDebugStringW(L"\n");
		}
		
		//去掉类相似弓字前面的两列空白象素
		if(pInfo->width>m_ui32Size/2&&pInfo->width<m_ui32Size)
		{
			int nBlankPix = 0;
			for(int i=0;i<pInfo->width;i++)
			{
				bool bBlank = true;
				for(int j=0;j<pInfo->height;j++)
				{
					if(pData[i*j]==0)
						bBlank = false;
				}
				if(bBlank)
					nBlankPix++;
				else
					break;
			}

			if(nBlankPix>0)
			{
				pInfo->width -= nBlankPix;
				uint *pDatatemp = new uint[pInfo->width * pInfo->height];
				for(int i=0;i<pInfo->width;i++)
				{
					for(int j=0;j<pInfo->height;j++)
					{
						pDatatemp[i*j] = pData[(i+nBlankPix)*j];
					}
				}
				delete [] pData;
				pData = pDatatemp;
			}
		}
		//pInfo->width = nNewWidth;
		//pInfo->height = nNewHeight;


		vTextures[pInfo->nTexIndex]->setSubData(0,pInfo->left,pInfo->top,pInfo->width,pInfo->height,pData);
		delete [] pBuf;
		delete [] pData;

	}

	//描边效果
	void PixTextureFont::effectGlyphToBuffer (CharTextureInfo * pInfo,wchar_t  szText,DWORD dwSize)
	{

		GLYPHMETRICS gm;
		MAT2 m2 = {
			{0, 1},
			{0, 0},
			{0, 0},
			{0, 1},
		};
		BYTE * pBuf = new BYTE[dwSize];//点阵数
		::GetGlyphOutlineW( m_hdc,szText, GGO_BITMAP, &gm, dwSize, pBuf, &m2);
		DWORD dwErr = GetLastError();
		if(dwSize==-1||dwSize==GDI_ERROR)
			return;

		int nWidth = gm.gmBlackBoxX;
		int nHeight = gm.gmBlackBoxY;

		//相对于原点的偏移
		int offsetx = gm.gmptGlyphOrigin.x;
		int offsety = m_textmetric.tmHeight-m_textmetric.tmDescent-gm.gmptGlyphOrigin.y;

		int nNewWidth = nWidth+offsetx;
		int nNewHeight = nHeight+offsety;
		if(nNewHeight<m_ui32Size)
		{
			nNewHeight = m_ui32Size;
			nNewWidth +=1;
		}

		uint *pData = new uint[(nNewWidth+2) * (nNewHeight+2)];//纹理数据
		memset(pData,0,sizeof(uint) * (nNewWidth+2) * (nNewHeight+2));

		
	
		//原始数据
		uint ncolor = fontcolor.GetValue();
		//if(ncolor!=0xffffffff)
		//	int i=0;
		int nStride = dwSize / nHeight;
		for(int y=0;y<nHeight;++y)//行 
		{
			for(int x=0;x<nStride;++x)//列
			{
				for(int k=0;k<8;++k)//每个字节 
				{
					if((pBuf[y*nStride+x]>>(7-k))&1) 
					{
						int nXPix = x*8+k;//本行的第几个象素
						if(nXPix<nWidth)
						{
							pData[(nNewWidth+2)*(y+1+offsety)+nXPix+1+offsetx] = ncolor;
						}
					}
				}
			}
		}

		uint  iefcolor = effectcolor.GetValue();


		//描边
		for(int y=1;y<nNewHeight+1;++y)//行 
		{
			for(int x=1;x<nNewWidth+1;++x)//列 
			{
				//每个象素
				if( pData[y*(nNewWidth+2)+x]==ncolor )
				{
					//八个方向描边
					if(pData[(y-1)*(nNewWidth+2)+x-1]==0)
						pData[(y-1)*(nNewWidth+2)+x-1] = iefcolor;
					if(pData[(y-1)*(nNewWidth+2)+x]==0)
						pData[(y-1)*(nNewWidth+2)+x] = iefcolor;
					if(pData[(y-1)*(nNewWidth+2)+x+1]==0)
						pData[(y-1)*(nNewWidth+2)+x+1] = iefcolor;

					if(pData[(y)*(nNewWidth+2)+x-1]==0)
						pData[(y)*(nNewWidth+2)+x-1] = iefcolor;
					//if(pData[(y)*(nNewWidth+2)+x]==0)
					//	pData[(y)*(nNewWidth+2)+x] = 0xff000000;
					if(pData[(y)*(nNewWidth+2)+x+1]==0)
						pData[(y)*(nNewWidth+2)+x+1] = iefcolor;

					if(pData[(y+1)*(nNewWidth+2)+x-1]==0)
						pData[(y+1)*(nNewWidth+2)+x-1] = iefcolor;
					if(pData[(y+1)*(nNewWidth+2)+x]==0)
						pData[(y+1)*(nNewWidth+2)+x] = iefcolor;
					if(pData[(y+1)*(nNewWidth+2)+x+1]==0)
						pData[(y+1)*(nNewWidth+2)+x+1] = iefcolor;

				}
			}
		}
		

		//去掉类相似弓字前面的两列空白象素
		if(pInfo->width>m_ui32Size/2&&pInfo->width<m_ui32Size)
		{
			int nBlankPix = 0;
			for(int i=0;i<pInfo->width;i++)
			{
				bool bBlank = true;
				for(int j=0;j<pInfo->height;j++)
				{
					if(pData[j*pInfo->width+i]!=0)
						bBlank = false;
				}
				if(bBlank)
					nBlankPix++;
				else
					break;
			}

			if(nBlankPix>0)
			{
				pInfo->width -= nBlankPix;
				uint *pDatatemp = new uint[pInfo->width * pInfo->height];
				for(int x=0;x<pInfo->width;x++)
				{
					for(int y=0;y<pInfo->height;y++)
					{
						pDatatemp[y*pInfo->width+x] = pData[y*(nBlankPix+pInfo->width)+x+nBlankPix];
					}
				}
				delete [] pData;
				pData = pDatatemp;
			}
		}

		

		vTextures[pInfo->nTexIndex]->setSubData(0,pInfo->left,pInfo->top,pInfo->width,pInfo->height,pData);
		
		delete [] pBuf;
		delete [] pData;
	}


}


