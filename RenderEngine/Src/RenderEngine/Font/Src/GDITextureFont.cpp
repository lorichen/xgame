

#include "StdAfx.h"
#include "GDITextureFont.h"

#undef new 

#define TEXTURE_SIZE	512

#if 1 //add by kevin.chen

namespace xs
{

	//产生字符图形数据
	bool GDITextureFont::GenerateTextBitmap(wchar_t * szText)
	{    
		StringFormat strformat;
		/*if(bPurple)
			m_PngOutlineText.TextOutline(Color(177,100,255),Color(128,0,255),1);
		else
			m_PngOutlineText.TextOutline(Color(125,190,255),Color(0,121,242),1); // blue outline*/

		//m_PngOutlineText.EnableShadow(false);
		m_PngOutlineText.SetPngImage(m_pbmp);
		m_PngOutlineText.DrawString(m_pGraphics,m_pFontFamily,FontStyleRegular, 
			m_ui32Size, szText, Gdiplus::Point(0,0), &strformat,fontcolor);

		return true;
	}
	
	//获取空白象素
	bool GDITextureFont::GetBlankPix( wchar_t * szText )
	{
		m_pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
		m_pGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
		m_pGraphics->SetPageUnit(UnitPixel);

		//清掉上次的脏数据
		for(uint row = 0;row < m_ui32Size;row++)
		{
			for(uint col = 0;col < m_ui32Size;col++)
			{
				m_pbmp->SetPixel(row,col,0);
			}
		}
		//

		GenerateTextBitmap(szText);

		Gdiplus::Rect rect1(0, 0, m_pbmp->GetWidth(), m_pbmp->GetHeight());
		// First Bitmap
		BitmapData bitmapData;
		memset( &bitmapData, 0, sizeof(bitmapData));
		m_pbmp->LockBits( 
			&rect1, 
			ImageLockModeRead,
			PixelFormat32bppARGB,
			&bitmapData );

		int nStride1 = bitmapData.Stride;
		if( nStride1 < 0 )
			nStride1 = -nStride1;

		UINT* pixels = (UINT*)bitmapData.Scan0;

		if( !pixels )
			return false;
		
		
		m_nBlankPixX = m_ui32Size;
		m_nBlankPixY = m_ui32Size;
		for(uint row = 0;row < m_ui32Size;row++)
		{
			for(uint col = 0;col < m_ui32Size;col++)
			{
				if(pixels[row * nStride1 / 4 + col]!=0)
				{
					if(m_nBlankPixX>col)
						m_nBlankPixX=col;
					if(m_nBlankPixY>row)
						m_nBlankPixY=row;
				}
			}
		}
		
		m_pbmp->UnlockBits( 
			&bitmapData );

		return true;
	}

	bool GDITextureFont::LoadOneTextBitmap( CharTextureInfo * pInfo,wchar_t * szText )
	{
		//m_pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
		//m_pGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
		//m_pGraphics->SetPageUnit(UnitPixel);

		//清掉上次的脏数据
		for(uint row = 0;row < pInfo->width+50;row++)
		{
			for(uint col = 0;col < pInfo->height+50;col++)
			{
				m_pbmp->SetPixel(row,col,0);
			}
		}
		//

		GenerateTextBitmap(szText);

		Gdiplus::Rect rect1(0, 0, m_pbmp->GetWidth(), m_pbmp->GetHeight());
		// First Bitmap
		BitmapData bitmapData;
		memset( &bitmapData, 0, sizeof(bitmapData));
		m_pbmp->LockBits( 
			&rect1, 
			ImageLockModeRead,
			PixelFormat32bppARGB,
			&bitmapData );

		int nStride1 = bitmapData.Stride;
		if( nStride1 < 0 )
			nStride1 = -nStride1;

		UINT* pixels = (UINT*)bitmapData.Scan0;

		if( !pixels )
			return false;

		
		//留一个象素
		int nRealBlankX = m_nBlankPixX-m_nFontThickness;
		if(nRealBlankX<0)
			nRealBlankX = 0;
		int nRealBlankY = m_nBlankPixY-m_nFontThickness;
		if(m_nBlankPixY<0)
			m_nBlankPixY = 0;

		//pInfo->width = pInfo->width-nRealBlankX+2;

		uint *pData = new uint[pInfo->width * pInfo->height];
		memset(pData,0,sizeof(uint) * pInfo->width * pInfo->height);
		for(uint row = 0;row < pInfo->height;row++)
		{
			for(uint col = 0;col < pInfo->width;col++)
			{
				UINT pixel = pixels[(row+m_nBlankPixY) * nStride1 / 4 + col];
				pData[row*pInfo->width + col] = pixel;
			}
		}


		m_pbmp->UnlockBits( 
			&bitmapData );

		vTextures[pInfo->nTexIndex]->setSubData(0,pInfo->left,pInfo->top,pInfo->width,pInfo->height,pData);
		delete [] pData;
	
		return true;

	}
	//升高值
	float GDITextureFont::Ascender()
	{
		return 0;//m_pFontFamily->GetCellAscent(FontStyleRegular);
	}
	//降低值
	float GDITextureFont::Descender()
	{
		return 0;//m_pFontFamily->GetCellDescent(FontStyleRegular);
	}
	//行距
	float GDITextureFont::LineHeight()
	{
		return m_ui32Size+m_nFontThickness*2;//m_pFontFamily->GetLineSpacing(FontStyleRegular);
	}

	//字符串的包围盒
	void  GDITextureFont::BBox( const wchar_t* wc, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)
	{
		float fHeight = 0;
		float fWidth = 0;
	
		while( *wc)
		{
			CharTextureInfo * pInfo = GetCharTextureInfo(*wc);
			if(pInfo==NULL)
				return;
			
			fWidth += pInfo->width;//-m_ui32Size/10;//宽度稍微做了下修正,让字和字挨紧点

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

	float GDITextureFont::Advance( const wchar_t* string)
	{
		return 0.0f;
	}
	//渲染一个字符号串
	void  GDITextureFont::Render(const wchar_t* c )
	{
		//SceneBlendFactor src,dst;
		//m_pRenderSystem->getSceneBlending(src,dst);
		//m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);

		//float fCurtop = 0;
		float fCurleft = 0;
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
			
			
			//mtxWorld.makeTrans(fCurleft, fCurtop, 0.0f);
			//m_pRenderSystem->setWorldMatrix(mtxWorldSave * mtxWorld);

			/*m_pRenderSystem->beginPrimitive(PT_QUADS);
			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[0].X, pInfo->uv[0].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft, fCurtop));

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[0].X, pInfo->uv[1].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft, fCurtop+pInfo->height ));

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[1].X, pInfo->uv[1].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft+pInfo->width, fCurtop+pInfo->height ));

			m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[1].X, pInfo->uv[0].Y));
			m_pRenderSystem->sendVertex(Vector2(fCurleft+pInfo->width, fCurtop));
			m_pRenderSystem->endPrimitive();*/

			
			
			fCurleft += pInfo->width;//-m_ui32Size/10;//宽度稍微做了下修正,让字和字挨紧点,一个字符串前面还有些空白
			//fCurtop += pInfo->height;

			++c;
		}
		//m_pRenderSystem->setWorldMatrix(mtxWorldSave);

		//m_pRenderSystem->setSceneBlending(src,dst);
		//m_pRenderSystem->setTexture(0,0);
	}
	void  GDITextureFont::RenderByScale(const wchar_t* c,float fScale)
	{
		float fCurleft = 0;
	
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
			renderinfo.scale = fScale;
			vCharInfoForTex[pInfo->nTexIndex].push_back(renderinfo);

			fCurleft += pInfo->width*fScale;

			++c;
		}
	}
	//字符串的包围盒
	void  GDITextureFont::BBox( const char* str, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)
	{
		xs::autostring wString(str);
		BBox(wString.c_wstr(),llx, lly, llz, urx, ury, urz);
	}
	float GDITextureFont::Advance( const char* string)
	{
		return 0.0f;
	}
	//渲染一个字符号串
	void  GDITextureFont::Render(const char* string )
	{
		Render(xs::autostring::to_wchar(string));
	}

	GDITextureFont::GDITextureFont():m_strFontName(),
		m_ui32Size(0),
		m_nFontThickness(0),
		m_pFontFamily(0),
		m_pbmp(0),
		m_pGraphics(0),
		m_nDrawTextleft(0),
		m_nDrawTextTop(0),
		fontcolor(255,255,255,255),
		m_pGradientBrush(0),
		m_nBlankPixX(0),
		m_nBlankPixY(0)
	{

	}

	bool GDITextureFont::create(IRenderSystem *pRenderSystem,const wchar_t* szFontName,uint size)
	{
		if( 0 == pRenderSystem)
			return false;

		if( 0 == pRenderSystem->getTextureManager())
			return false;

		std::wstring fontName = szFontName;
		if( fontName.empty() )
			return false;

		m_pRenderSystem = pRenderSystem;
		m_pTextureManager = pRenderSystem->getTextureManager();
		m_ui32Size = size;
		m_strFontName = fontName;

		//
		m_pFontFamily = new FontFamily(szFontName);
		if( m_pFontFamily->GetLastStatus() == FontFamilyNotFound )
		{
			PrivateFontCollection pfc;
			char szSourceFile[MAX_PATH+16];
			lstrcpyn( szSourceFile, xs::getWorkDir(),MAX_PATH );
			strcat(szSourceFile,"\\fonts\\STXINGKA.TTF");
			pfc.AddFontFile(xs::autostring::to_wchar(szSourceFile));

			// Get the array of FontFamily objects. 
			int count = pfc.GetFamilyCount();
			int found;
			if(count==1)
			{
				pfc.GetFamilies(count,m_pFontFamily,&found);
			}
			else
			{
				Error("PrivateFontCollection::AddFontFile failed");
				return false;
			}
		}


		m_pbmp = new Gdiplus::Bitmap(256,256,PixelFormat32bppARGB);
		m_pGraphics = new Graphics(m_pbmp);

		CharInfoForTex ci;
		vCharInfoForTex.push_back(ci);

		ITexture* pTexture = m_pTextureManager->createEmptyTextureNoWait(TEXTURE_SIZE,TEXTURE_SIZE,PF_A8R8G8B8,FO_LINEAR,FO_LINEAR,FO_NONE);
		if( 0  == pTexture )
			return false;
		vTextures.push_back( pTexture );

		//为了得出空白象素
		GetBlankPix(L"宋");

	

		return true;
	}


	//GDITextureFont::GDITextureFont(IRenderSystem *pRenderSystem,ITextureManager *pTextureManager,const wchar_t * szFontName,uint size)
	//{
	//	m_nFontThickness = 0;
	//	m_pGradientBrush = NULL;
	//	m_pRenderSystem =  pRenderSystem;
	//	m_pTextureManager =  pTextureManager;

	//	/*
	//	InstalledFontCollection ifc;
	//	int count = ifc.GetFamilyCount();
	//	int found;
	//	FontFamily * pFontFamily = new FontFamily[count];
	//	ifc.GetFamilies(count,pFontFamily,&found);
	//
	//	for(int i = 0; i < count; i++) {
	//		wchar_t family_name[LF_FACESIZE];
	//		pFontFamily[i].GetFamilyName(family_name);
	//		::OutputDebugStringW(family_name);
	//		::OutputDebugStringW(L"\n");
	//	}*/
	//	m_pFontFamily = new FontFamily(  szFontName );//

	//	if( m_pFontFamily->GetLastStatus() == FontFamilyNotFound )
	//	{
	//		PrivateFontCollection pfc;
	//		char szSourceFile[MAX_PATH];
	//		::GetCurrentDirectory(MAX_PATH,szSourceFile);
	//		strcat(szSourceFile,"\\fonts\\STXINGKA.TTF");
	//		pfc.AddFontFile(xs::autostring::to_wchar(szSourceFile));

	//		// Get the array of FontFamily objects. 

	//		int count = pfc.GetFamilyCount();
	//		int found;
	//		if(count==1)
	//		{
	//			pfc.GetFamilies(count,m_pFontFamily,&found);
	//		}
	//		else
	//		{
	//			Error("PrivateFontCollection::AddFontFile failed");
	//		}
	//		//
	//		/*
	//		delete m_pFontFamily;
	//		//重新再创建
	//		m_pFontFamily = new FontFamily(  szFontName );

	//		//实在不行就用宋体了
	//		if( m_pFontFamily->GetLastStatus() == FontFamilyNotFound )
	//		{
	//			delete m_pFontFamily;
	//			m_pFontFamily = new FontFamily(  L"宋体" );
	//		}*/
	//	}
	//	//m_pFontFamily = new FontFamily( xs::autostring::to_wchar(szFontName));
	//	m_ui32Size = size;

	//	m_pbmp = new Gdiplus::Bitmap(256,256,PixelFormat32bppARGB);
	//	m_pGraphics = new Graphics(m_pbmp);

	//	m_nDrawTextleft = 0;
	//	m_nDrawTextTop = 0;
	//	m_nBlankPixX = 0;
	//	m_nBlankPixY = 0;
	//	fontcolor.SetValue(0xffffffff);

	//	vTextures.push_back( m_pTextureManager->createEmptyTexture(TEXTURE_SIZE,TEXTURE_SIZE,PF_A8R8G8B8,FO_LINEAR,FO_LINEAR,FO_NONE) );
	//	CharInfoForTex ci;
	//	vCharInfoForTex.push_back(ci);
	//
	//	//默认是字是白色,无描边
	//	//fontcolor = Color(255,255,255);
	//	//m_PngOutlineText.TextOutline(fontcolor,Color(0,0,0),0);
	//	
	//	//GradientText(Color(255,255,255),Color(255,255,255));
	//	//TextOutline(Color(0,255,0),6);
	//	//为了得出空白象素
	//	GetBlankPix(L"宋");
	//	//TextDblOutline(Color(0,255,0),Color(0,255,0),1,3);
	//	/*Gdiplus::LinearGradientBrush * pGradientBrush = new LinearGradientBrush(Gdiplus::Rect(0,0, 50, 50), 
	//		Color(0,0,0),
	//		Color(0,0,0),
	//		LinearGradientModeVertical);
	//	m_PngOutlineText.TextOutline(pGradientBrush,Color(255,255,255),2);
	//	m_PngOutlineText.EnableShadow(true);
	//	m_PngOutlineText.Extrude(
	//		Gdiplus::Color(128,0,0,0), 
	//		4,
	//		Gdiplus::Point(0,0));*/

	//}

	void GDITextureFont::release()
	{
		safeDelete( m_pFontFamily);
		safeDelete( m_pbmp);
		safeDelete(m_pGraphics);
		safeDelete(m_pGradientBrush);

		delete this;

	}

	//GDITextureFont::~GDITextureFont()
	//{
	//	if(m_pFontFamily)
	//		delete m_pFontFamily;
	//	if(m_pbmp)
	//		delete m_pbmp;
	//	if(m_pGraphics)
	//		delete m_pGraphics;
	//	if(m_pGradientBrush)
	//		delete m_pGradientBrush;
	//}


	FontRenderDirection GDITextureFont::setRenderDirection( FontRenderDirection rd)
	{
		return rd;
	}
	float GDITextureFont::setFontGap( float gap)
	{
		return 0.0f;
	}
	
	//  获取渲染时用到的纹理信息
	CharTextureInfo * GDITextureFont::GetCharTextureInfo(wchar_t wc)
	{
		FontTexture::iterator it = m_FontTexture.find(wc);
		if(it != m_FontTexture.end())
			return &(*it).second;

		//看看这个字符需要多大区域
		static CharTextureInfo info;

		float fWidth=0.0f;
		float fHeight=0.0f;
		StringFormat strformat;

		wchar_t wctemp[2];
		wctemp[0] = wc;
		wctemp[1] = 0;
		
		
		m_PngOutlineText.MeasureString(m_pGraphics,m_pFontFamily,FontStyleRegular, 
			m_ui32Size, wctemp, Gdiplus::Point(0,0), &strformat,
			&fWidth, &fHeight);
		
		if(wc==' ')
		{
			wctemp[0] = '1';
			m_PngOutlineText.MeasureString(m_pGraphics,m_pFontFamily,FontStyleRegular, 
				m_ui32Size, wctemp, Gdiplus::Point(0,0), &strformat,
				&fWidth, &fHeight);
			wctemp[0]=' ';
		}
	
		if( m_nDrawTextleft + fWidth >TEXTURE_SIZE-2 )
		{
			//换行
			if( m_nDrawTextTop+fHeight>TEXTURE_SIZE-2 )
			{
				vTextures.push_back( m_pTextureManager->createEmptyTextureNoWait(TEXTURE_SIZE,TEXTURE_SIZE,PF_A8R8G8B8,FO_LINEAR,FO_LINEAR,FO_NONE) );
				CharInfoForTex ci;
				vCharInfoForTex.push_back(ci);
				m_nDrawTextleft = 0;
				m_nDrawTextTop = 0;
				info.left = 0;
				info.top = 0;
			}
			else
			{
				m_nDrawTextleft = fWidth+5;
				m_nDrawTextTop = m_nDrawTextTop+fHeight+20;
				info.left = 0;
				info.top = m_nDrawTextTop;
			}
		}
		else
		{
			info.left = m_nDrawTextleft;
			info.top = m_nDrawTextTop;
			m_nDrawTextleft = m_nDrawTextleft+fWidth+5;
		}
		
		
		fWidth+=2;
		fHeight+=2;

		info.width = fWidth;
		info.height = fHeight;
		info.nTexIndex = vTextures.size() -1;


		info.uv[0].X =  static_cast<float>(info.left) / TEXTURE_SIZE;
		info.uv[0].Y =  static_cast<float>(info.top) / TEXTURE_SIZE;
		info.uv[1].X =  static_cast<float>(info.left+info.width) / TEXTURE_SIZE;
		info.uv[1].Y =  static_cast<float>(m_nDrawTextTop +info.height) / TEXTURE_SIZE;

		m_FontTexture[wc] = info;

		//将本字符的图形放到对应的区域
		LoadOneTextBitmap(&info,wctemp);
			
		return &info;
	}

	//颜色渐变
	void GDITextureFont::GradientText(Color color1,Color color2)
	{
		m_pGradientBrush = new LinearGradientBrush(Gdiplus::Rect(0,0, m_ui32Size, m_ui32Size), 
			color1,
			color2,
			LinearGradientModeVertical);

		GetBlankPix(L"宋");
	}


	//单描边
	void GDITextureFont::TextOutline(Gdiplus::Color clrOutline, 
		int nThickness)
	{
		if(m_nFontThickness<nThickness)
			m_nFontThickness=nThickness;
		if(m_pGradientBrush)
		{
			m_PngOutlineText.TextOutline(m_pGradientBrush,clrOutline,nThickness);
		}
		else
		{
			m_PngOutlineText.TextOutline(fontcolor,clrOutline,nThickness);
		}
		GetBlankPix(L"宋");
	}
	//双描边
	void GDITextureFont::TextDblOutline(Gdiplus::Color clrOutline1, 
		Gdiplus::Color clrOutline2, 
		int nThickness1, 
		int nThickness2)
	{
		if(m_nFontThickness<nThickness1+nThickness2)
			m_nFontThickness = nThickness1+nThickness2;
		if(m_pGradientBrush)
		{
			m_PngOutlineText.TextDblOutline(m_pGradientBrush,clrOutline1,clrOutline2,nThickness1,nThickness2);
		}
		else
		{
			m_PngOutlineText.TextDblOutline(fontcolor,clrOutline1,clrOutline2,nThickness1,nThickness2);
		}
		
		GetBlankPix(L"宋");		
	}

	//发光
	void GDITextureFont::TextGlow(
		Gdiplus::Color clrOutline, 
		int nThickness)
	{
		if(m_nFontThickness<nThickness)
			m_nFontThickness=nThickness;
		if(m_pGradientBrush)
		{
			m_PngOutlineText.TextGlow(m_pGradientBrush,clrOutline,nThickness);
		}
		else
		{
			m_PngOutlineText.TextGlow(fontcolor,clrOutline,nThickness);
		}
		GetBlankPix(L"宋");
	}
	//阴影
	void GDITextureFont::Shadow(
		Gdiplus::Color color, 
		int nThickness,
		Gdiplus::Point ptOffset)
	{
		if(m_nFontThickness<nThickness)
			m_nFontThickness=nThickness;
		m_PngOutlineText.Shadow(color,nThickness,ptOffset);

		GetBlankPix(L"宋");
	}
	//发散的阴影
	void GDITextureFont::DiffusedShadow(
		Gdiplus::Color color, 
		int nThickness,
		Gdiplus::Point ptOffset)
	{
		if(m_nFontThickness<nThickness)
			m_nFontThickness=nThickness;
		m_PngOutlineText.DiffusedShadow(color,nThickness,ptOffset);
		GetBlankPix(L"宋");
	}
	//突出字,阴影在后
	void GDITextureFont::Extrude(
		Gdiplus::Color color, 
		int nThickness,
		Gdiplus::Point ptOffset)
	{
		if(m_nFontThickness<nThickness)
			m_nFontThickness=nThickness;
		m_PngOutlineText.Extrude(color,nThickness,ptOffset);

		GetBlankPix(L"宋");
	}

	//预初始化一些常用字
	void GDITextureFont::InitText(wchar_t *pStr)
	{		
		if( NULL == pStr)
			return;

		while(*pStr)
		{
			GetCharTextureInfo(*pStr);	
			++pStr;
		}
	}
}

#endif

