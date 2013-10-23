#include "StdAfx.h"
#include "Viewport.h"
#include "Camera.h"

namespace xs
{

	void  Viewport::release()
	{
		delete this;
	}

	Viewport::Viewport()
	{
		m_ui32Left = m_ui32Top = 0;
		m_ui32Width = m_ui32Height = 1;

		m_bClear = true;
		m_color = ColorValue(1.0f,1.0f,1.0f,0.0f);

		m_pCamera = 0;
	}

	Viewport::~Viewport()
	{
	}

	int Viewport::getLeft()
	{
		return m_ui32Left;
	}

	int Viewport::getTop()
	{
		return m_ui32Top;
	}

	int Viewport::getWidth()
	{
		return m_ui32Width;
	}

	int Viewport::getHeight()
	{
		return m_ui32Height;
	}

	void  Viewport::setLeft(int	left)
	{
		m_ui32Left = left;
		if(m_pCamera)m_pCamera->setNeedUpdate(true);
	}

	void  Viewport::setTop(int top)
	{
		m_ui32Top = top;
		if(m_pCamera)m_pCamera->setNeedUpdate(true);
	}

	void  Viewport::setWidth(int width)
	{
		m_ui32Width = width;
		if(m_pCamera)m_pCamera->setNeedUpdate(true);
	}
	void  Viewport::setHeight(int height)
	{
		m_ui32Height = height;
		if(m_pCamera)m_pCamera->setNeedUpdate(true);
	}
	void  Viewport::setValues(int left,int top,int width,int height)
	{
		m_ui32Left = left;
		m_ui32Top = top;
		m_ui32Width = width;
		m_ui32Height = height;

		if(m_pCamera)m_pCamera->setNeedUpdate(true);
	}
	void  Viewport::getValues(int& left,int& top,int& width,int& height)
	{
		left = m_ui32Left;
		top = m_ui32Top;
		width = m_ui32Width;
		height = m_ui32Height;
	}

	void  Viewport::setBackgroundColor(const ColorValue& color)
	{
		m_color = color;
	}

	const ColorValue& Viewport::getBackgroundColor(void ) const
	{
		return m_color;
	}

	void  Viewport::setClearEveryFrame(bool clear)
	{
		m_bClear = clear;
	}

	bool Viewport::getClearEveryFrame(void ) const
	{
		return m_bClear;
	}

	void  Viewport::setCamera(Camera *pCamera)
	{
		m_pCamera = pCamera;
	}
}