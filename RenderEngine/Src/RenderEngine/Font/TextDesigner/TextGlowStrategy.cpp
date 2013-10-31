/*
Text Designer Outline Text Library 0.3.0

Copyright (c) 2009 Wong Shao Voon

The Code Project Open License (CPOL)
http://www.codeproject.com/info/cpol10.aspx
*/

#include "../../StdAfx.h"
#include "TextGlowStrategy.h"
#include "GDIPath.h"

using namespace TextDesigner;

TextGlowStrategy::TextGlowStrategy(void)
:
m_nThickness(8),
m_pbrushText(NULL),
m_bClrText(true)
{
}

TextGlowStrategy::~TextGlowStrategy(void)
{
}

void TextGlowStrategy::Init(
	Gdiplus::Color clrText, 
	Gdiplus::Color clrOutline, 
	int nThickness)
{
	m_clrText = clrText; 
	m_bClrText = true;
	m_clrOutline = clrOutline; 
	m_nThickness = nThickness; 
}

void TextGlowStrategy::Init(
	Gdiplus::Brush* pbrushText, 
	Gdiplus::Color clrOutline, 
	int nThickness)
{
	m_pbrushText = pbrushText; 
	m_bClrText = false;
	m_clrOutline = clrOutline; 
	m_nThickness = nThickness; 
}

bool TextGlowStrategy::DrawString(
	Gdiplus::Graphics* pGraphics, 
	Gdiplus::FontFamily* pFontFamily,
	Gdiplus::FontStyle fontStyle,
	int nfontSize,
	const wchar_t*pszText, 
	Gdiplus::Point ptDraw, 
	Gdiplus::StringFormat* pStrFormat)
{
	using namespace Gdiplus;
	GraphicsPath path;
	Status status = path.AddString(pszText,wcslen(pszText),pFontFamily,fontStyle,nfontSize,ptDraw,pStrFormat);
	if(status!=Ok)
		return false;

	for(int i=1; i<=m_nThickness; ++i)
	{
		Pen pen(m_clrOutline,i);
		pen.SetLineJoin(LineJoinRound);
		pGraphics->DrawPath(&pen, &path);
	}
	
	Status status2 = Ok;
	if(m_bClrText)
	{
		SolidBrush brush(m_clrText);
		status2 = pGraphics->FillPath(&brush, &path);
	}
	else
	{
		status2 = pGraphics->FillPath(m_pbrushText, &path);
	}

	return status2 == Ok;
}

bool TextGlowStrategy::DrawString(
	Gdiplus::Graphics* pGraphics, 
	Gdiplus::FontFamily* pFontFamily,
	Gdiplus::FontStyle fontStyle,
	int nfontSize,
	const wchar_t*pszText, 
	Gdiplus::Rect rtDraw, 
	Gdiplus::StringFormat* pStrFormat)
{
	using namespace Gdiplus;
	GraphicsPath path;
	Status status = path.AddString(pszText,wcslen(pszText),pFontFamily,fontStyle,nfontSize,rtDraw,pStrFormat);
	if(status!=Ok)
		return false;

	for(int i=1; i<=m_nThickness; ++i)
	{
		Pen pen(m_clrOutline,i);
		pen.SetLineJoin(LineJoinRound);
		pGraphics->DrawPath(&pen, &path);
	}

	Status status2 = Ok;
	if(m_bClrText)
	{
		SolidBrush brush(m_clrText);
		status2 = pGraphics->FillPath(&brush, &path);
	}
	else
	{
		status2 = pGraphics->FillPath(m_pbrushText, &path);
	}

	return status2 == Ok;
}

bool TextGlowStrategy::GdiDrawString(
	Gdiplus::Graphics* pGraphics, 
	LOGFONTW* pLogFont,
	const wchar_t*pszText, 
	Gdiplus::Point ptDraw)
{
	using namespace Gdiplus;
	Gdiplus::GraphicsPath* pPath=NULL;
	bool b = GDIPath::GetStringPath(
		pGraphics, 
		&pPath, 
		pszText, 
		pLogFont,
		ptDraw);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	for(int i=1; i<=m_nThickness; ++i)
	{
		Pen pen(m_clrOutline,i);
		pen.SetLineJoin(LineJoinRound);
		pGraphics->DrawPath(&pen, pPath);
	}

	Status status2 = Ok;
	if(m_bClrText)
	{
		SolidBrush brush(m_clrText);
		status2 = pGraphics->FillPath(&brush, pPath);
	}
	else
	{
		status2 = pGraphics->FillPath(m_pbrushText, pPath);
	}

	if(pPath)
	{
		delete pPath;
		pPath = NULL;
	}
	return status2 == Ok;
}

bool TextGlowStrategy::GdiDrawString(
	Gdiplus::Graphics* pGraphics, 
	LOGFONTW* pLogFont,
	const wchar_t*pszText, 
	Gdiplus::Rect rtDraw)
{
	using namespace Gdiplus;
	Gdiplus::GraphicsPath* pPath=NULL;
	bool b = GDIPath::GetStringPath(
		pGraphics, 
		&pPath, 
		pszText, 
		pLogFont,
		rtDraw);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	for(int i=1; i<=m_nThickness; ++i)
	{
		Pen pen(m_clrOutline,i);
		pen.SetLineJoin(LineJoinRound);
		pGraphics->DrawPath(&pen, pPath);
	}

	Status status2 = Ok;
	if(m_bClrText)
	{
		SolidBrush brush(m_clrText);
		status2 = pGraphics->FillPath(&brush, pPath);
	}
	else
	{
		status2 = pGraphics->FillPath(m_pbrushText, pPath);
	}

	if(pPath)
	{
		delete pPath;
		pPath = NULL;
	}
	return status2 == Ok;
}

bool TextGlowStrategy::MeasureString(
	Gdiplus::Graphics* pGraphics, 
	Gdiplus::FontFamily* pFontFamily,
	Gdiplus::FontStyle fontStyle,
	int nfontSize,
	const wchar_t*pszText, 
	Gdiplus::Point ptDraw, 
	Gdiplus::StringFormat* pStrFormat,
	float* pfDestWidth,
	float* pfDestHeight )
{
	using namespace Gdiplus;
	GraphicsPath path;
	Status status = path.AddString(pszText,wcslen(pszText),pFontFamily,fontStyle,nfontSize,ptDraw,pStrFormat);
	if(status!=Ok)
		return false;

	*pfDestWidth= ptDraw.X;
	*pfDestHeight= ptDraw.Y;
	bool b = GDIPath::MeasureGraphicsPath(pGraphics, &path, pfDestWidth, pfDestHeight);

	if(false==b)
		return false;

	float pixelThick = 0.0f;
	b = GDIPath::ConvertToPixels(pGraphics,m_nThickness,0.0f,&pixelThick,NULL);

	if(false==b)
		return false;

	*pfDestWidth += pixelThick;
	*pfDestHeight += pixelThick;

	return true;
}

bool TextGlowStrategy::MeasureString(
	Gdiplus::Graphics* pGraphics, 
	Gdiplus::FontFamily* pFontFamily,
	Gdiplus::FontStyle fontStyle,
	int nfontSize,
	const wchar_t*pszText, 
	Gdiplus::Rect rtDraw,
	Gdiplus::StringFormat* pStrFormat,
	float* pfDestWidth,
	float* pfDestHeight )
{
	using namespace Gdiplus;
	GraphicsPath path;
	Status status = path.AddString(pszText,wcslen(pszText),pFontFamily,fontStyle,nfontSize,rtDraw,pStrFormat);
	if(status!=Ok)
		return false;

	*pfDestWidth= rtDraw.GetLeft();
	*pfDestHeight= rtDraw.GetTop();
	bool b = GDIPath::MeasureGraphicsPath(pGraphics, &path, pfDestWidth, pfDestHeight);

	if(false==b)
		return false;

	float pixelThick = 0.0f;
	b = GDIPath::ConvertToPixels(pGraphics,m_nThickness,0.0f,&pixelThick,NULL);

	if(false==b)
		return false;

	*pfDestWidth += pixelThick;
	*pfDestHeight += pixelThick;

	return true;
}

bool TextGlowStrategy::GdiMeasureString(
	Gdiplus::Graphics* pGraphics, 
	LOGFONTW* pLogFont,
	const wchar_t*pszText, 
	Gdiplus::Point ptDraw,
	float* pfDestWidth,
	float* pfDestHeight )
{
	using namespace Gdiplus;
	Gdiplus::GraphicsPath* pPath=NULL;
	bool b = GDIPath::GetStringPath(
		pGraphics, 
		&pPath, 
		pszText, 
		pLogFont,
		ptDraw);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	*pfDestWidth= ptDraw.X;
	*pfDestHeight= ptDraw.Y;
	b = GDIPath::MeasureGraphicsPath(pGraphics, pPath, pfDestWidth, pfDestHeight);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	float pixelThick = 0.0f;
	b = GDIPath::ConvertToPixels(pGraphics,m_nThickness,0.0f,&pixelThick,NULL);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	*pfDestWidth += pixelThick;
	*pfDestHeight += pixelThick;

	if(pPath)
	{
		delete pPath;
		pPath = NULL;
	}
	return true;
}

bool TextGlowStrategy::GdiMeasureString(
	Gdiplus::Graphics* pGraphics, 
	LOGFONTW* pLogFont,
	const wchar_t*pszText, 
	Gdiplus::Rect rtDraw,
	float* pfDestWidth,
	float* pfDestHeight )
{
	using namespace Gdiplus;
	Gdiplus::GraphicsPath* pPath=NULL;
	bool b = GDIPath::GetStringPath(
		pGraphics, 
		&pPath, 
		pszText, 
		pLogFont,
		rtDraw);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	*pfDestWidth= rtDraw.GetLeft();
	*pfDestHeight= rtDraw.GetTop();
	b = GDIPath::MeasureGraphicsPath(pGraphics, pPath, pfDestWidth, pfDestHeight);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	float pixelThick = 0.0f;
	b = GDIPath::ConvertToPixels(pGraphics,m_nThickness,0.0f,&pixelThick,NULL);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	*pfDestWidth += pixelThick;
	*pfDestHeight += pixelThick;

	if(pPath)
	{
		delete pPath;
		pPath = NULL;
	}
	return true;
}

bool TextGlowStrategy::GdiMeasureStringRealHeight(
	Gdiplus::Graphics* pGraphics, 
	LOGFONTW* pLogFont,
	const wchar_t*pszText, 
	Gdiplus::Point ptDraw,
	float* pfDestWidth,
	float* pfDestHeight )
{
	using namespace Gdiplus;
	Gdiplus::GraphicsPath* pPath=NULL;
	bool b = GDIPath::GetStringPath(
		pGraphics, 
		&pPath, 
		pszText, 
		pLogFont,
		ptDraw);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	*pfDestWidth= ptDraw.X;
	*pfDestHeight= ptDraw.Y;
	b = GDIPath::MeasureGraphicsPathRealHeight(pGraphics, pPath, pfDestWidth, pfDestHeight);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	float pixelThick = 0.0f;
	b = GDIPath::ConvertToPixels(pGraphics,m_nThickness,0.0f,&pixelThick,NULL);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	*pfDestWidth += pixelThick;
	*pfDestHeight += pixelThick;

	if(pPath)
	{
		delete pPath;
		pPath = NULL;
	}
	return true;
}

bool TextGlowStrategy::GdiMeasureStringRealHeight(
	Gdiplus::Graphics* pGraphics, 
	LOGFONTW* pLogFont,
	const wchar_t*pszText, 
	Gdiplus::Rect rtDraw,
	float* pfDestWidth,
	float* pfDestHeight )
{
	using namespace Gdiplus;
	Gdiplus::GraphicsPath* pPath=NULL;
	bool b = GDIPath::GetStringPath(
		pGraphics, 
		&pPath, 
		pszText, 
		pLogFont,
		rtDraw);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	*pfDestWidth= rtDraw.GetLeft();
	*pfDestHeight= rtDraw.GetTop();
	b = GDIPath::MeasureGraphicsPathRealHeight(pGraphics, pPath, pfDestWidth, pfDestHeight);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	float pixelThick = 0.0f;
	b = GDIPath::ConvertToPixels(pGraphics,m_nThickness,0.0f,&pixelThick,NULL);

	if(false==b)
	{
		if(pPath)
		{
			delete pPath;
			pPath = NULL;
		}
		return false;
	}

	*pfDestWidth += pixelThick;
	*pfDestHeight += pixelThick;

	if(pPath)
	{
		delete pPath;
		pPath = NULL;
	}
	return true;
}