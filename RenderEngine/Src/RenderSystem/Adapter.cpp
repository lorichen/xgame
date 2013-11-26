#include "StdAfx.h"

#ifdef _WIN32

#include "windows.h"
#include "Adapter.h"

namespace xs{

Adapter::Adapter()
{
	m_bWndMode = TRUE;

	m_OldDevMode.dmFields |= (DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY);
	m_OldDevMode.dmSize = sizeof(DEVMODE);

	EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&m_OldDevMode);
}
Adapter::~Adapter()
{
	//if(!m_bWndMode)ChangeDisplaySettings(&m_OldDevMode,CDS_FULLSCREEN);
}
bool Adapter::fullScreen(DEVMODE& RecDevMode)
{
	//Set the dmFields member of a DEVMODE structure to DM_DISPLAYORIENTATION. 
	//Call ChangeDisplaySettingsEx with the lpMode parameter set to the DEVMODE structure and the dwFlags parameter set to CDS_TEST. The following code example shows how to call ChangeDisplaySettingsEx. 
	//ChangeDisplaySettingsEx(NULL, &devMode, NULL, CDS_TEST, NULL);
/*	DEVMODE dm;
	memset(&dm,0,sizeof(dm));
	dm.dmFields = DM_DISPLAYORIENTATION;
	dm.dmSize = sizeof(dm);
	if(DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettingsEx(NULL, &dm, NULL, CDS_TEST, NULL))
	{
		switch(dm.dmDisplayOrientation)
		{
		case DMDO_DEFAULT:
			break;
		case DMDO_90:
			RecDevMode.dmDisplayOrientation = DMDO_270;
			RecDevMode.dmFields |= DM_DISPLAYORIENTATION;
			break;
		case DMDO_180:
			RecDevMode.dmDisplayOrientation = DMDO_180;
			RecDevMode.dmFields |= DM_DISPLAYORIENTATION;
			break;
		case DMDO_270:
			RecDevMode.dmDisplayOrientation = DMDO_90;
			RecDevMode.dmFields |= DM_DISPLAYORIENTATION;
			break;
		}
	}*/
	RecDevMode.dmDisplayOrientation = DMDO_DEFAULT;
	RecDevMode.dmFields |= DM_DISPLAYORIENTATION;

	long lRet = ChangeDisplaySettings(&RecDevMode, CDS_FULLSCREEN);
	return DISP_CHANGE_SUCCESSFUL == lRet;
}
bool Adapter::testMode(DEVMODE& dm)
{
	dm.dmFields |= (DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY);
	long lRet = ChangeDisplaySettings(&dm, CDS_TEST);
	return DISP_CHANGE_SUCCESSFUL == lRet;
}
bool Adapter::toggleFullScreen(uint nWidth, uint nHeight, uint nColorDepth, uint nFrequency)
{
	DEVMODE DevMode;
	DEVMODE RecDevMode;
	uint nIndex = 0;
	RecDevMode.dmDisplayFrequency	= 0;
	bool bHaveOne	= false;

	//win9x下，取到的刷新率是0 - yun
	DWORD dwStandard = (m_OldDevMode.dmDisplayFrequency > 0 ? m_OldDevMode.dmDisplayFrequency : nFrequency);

	//Trace("OriginFreq = %d\n",m_OldDevMode.dmDisplayFrequency);

	while( EnumDisplaySettings(NULL,nIndex,&DevMode)!=0 )
	{
		if(DevMode.dmPelsWidth == (DWORD)nWidth && DevMode.dmPelsHeight == (DWORD)nHeight && DevMode.dmBitsPerPel == (DWORD)nColorDepth)
		{
			//Trace("%d*%d*%d %d\n",DevMode.dmPelsWidth,DevMode.dmPelsHeight,DevMode.dmBitsPerPel,DevMode.dmDisplayFrequency);
			if(DevMode.dmDisplayFrequency <= dwStandard)
			{
				if(testMode(DevMode))
				{
					//Trace("TestMode Ok! %d*%d*%d %d\n",DevMode.dmPelsWidth,DevMode.dmPelsHeight,DevMode.dmBitsPerPel,DevMode.dmDisplayFrequency);
					if(DevMode.dmDisplayFrequency >= RecDevMode.dmDisplayFrequency)
					{
						memcpy(&RecDevMode,&DevMode,sizeof(DEVMODE));
						bHaveOne	= true;
					}
				}
			}
		}
		nIndex++;
	}

	bool bRet = FALSE;
	if(bHaveOne)
	{
		if(RecDevMode.dmDisplayFrequency == 0)
			RecDevMode.dmDisplayFrequency = nFrequency;

		bRet = fullScreen(RecDevMode);

		//Trace("DEVMODE = %d*%d*%d %d",DevMode.dmPelsWidth,DevMode.dmPelsHeight,DevMode.dmBitsPerPel,DevMode.dmDisplayFrequency);
	}

	return bRet;
}

bool Adapter::toggleBack()
{
	return DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&m_OldDevMode,CDS_FULLSCREEN);
}

//获取当前的显示模式
bool Adapter::getCurrentMode(void *hWnd, bool &bFullScreen, uint &nWidth, uint & nHeight, uint &nColorDepth, uint & nFrequency)
{
	bFullScreen = !m_bWndMode;

	if( bFullScreen)
	{
		DEVMODE dm;
		EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dm);
		nColorDepth = dm.dmBitsPerPel;
		nFrequency = dm.dmDisplayFrequency;
		nWidth = dm.dmPelsWidth;
		nHeight = dm.dmPelsHeight;
	}
	else
	{
		RECT rc;
		GetClientRect((HWND)hWnd,&rc);
		nWidth = rc.right - rc.left;
		nHeight = rc.bottom -rc.top;

		DEVMODE dm;
		EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dm);
		nColorDepth = dm.dmBitsPerPel;
		nFrequency = dm.dmDisplayFrequency;
	}
	
	return true;
}

// 在两个分辨间切换
// 返回：窗口模式返回TRUE，否则返回FALSE
bool Adapter::toggleMode(void* hwnd,bool bFullScreen,uint nWidth, uint nHeight, uint nColorDepth, uint nFrequency)
{
	static RECT mainRect;
	bool bOk = FALSE;

	RECT rc;
	GetClientRect((HWND)hwnd,&rc);
	DEVMODE dm;
	EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dm);

	if(m_bWndMode == !bFullScreen && nWidth == rc.right - rc.left && nHeight == rc.bottom - rc.top)
	{
		if(m_bWndMode)return true;
		else if(nColorDepth == dm.dmBitsPerPel && nFrequency == dm.dmDisplayFrequency)return true;
	}

	HWND hWnd = (HWND)hwnd;
	if(bFullScreen)
	{
		if(toggleFullScreen(nWidth,nHeight,nColorDepth,nFrequency))
		{
			GetWindowRect(hWnd,&mainRect);
			// remove the caption of the mainWnd:
			LONG style=::GetWindowLong(hWnd,GWL_STYLE);
			style &= ~WS_CAPTION;
			style &= ~WS_BORDER;
			style &= ~WS_SIZEBOX;
			::SetWindowLong(hWnd,GWL_STYLE,style);

			uint screenx=GetSystemMetrics(SM_CXSCREEN);
			uint screeny=GetSystemMetrics(SM_CYSCREEN);
			uint xTop = GetSystemMetrics(SM_CXDLGFRAME) + GetSystemMetrics(SM_CXBORDER) - 1;
			uint yTop = GetSystemMetrics(SM_CYDLGFRAME) + GetSystemMetrics(SM_CYBORDER) - 1;
			// resize:
			SetWindowPos(hWnd,NULL,/*-xTop*/0,/*-yTop*/0,screenx/*+8*/,screeny/*+8*/,SWP_NOZORDER);

			m_bWndMode = FALSE;
			bOk = TRUE;
		}
	}
	else
	{
		BOOL bOk = TRUE;
		if(!m_bWndMode)bOk = toggleBack();
		if(bOk)
		{
			LONG style=::GetWindowLong(hWnd,GWL_STYLE);
			style|=WS_CAPTION;
			style|=WS_BORDER;
			style|=WS_SIZEBOX;
			SetLastError(0);
			long l = ::SetWindowLong(hWnd,GWL_STYLE,style);
			DWORD dw = GetLastError();

			//MoveWindow(hWnd,mainRect.left,mainRect.top,mainRect.right - mainRect.left,mainRect.bottom - mainRect.top,TRUE);
			DWORD	dwScreenWidth=GetSystemMetrics(SM_CXSCREEN);
			DWORD	dwScreenHeight=GetSystemMetrics(SM_CYSCREEN);
			RECT rcWindow = {0,0,nWidth,nHeight};
			dw = GetWindowLong(hWnd,GWL_STYLE);
			HMENU h = GetMenu(hWnd);
			AdjustWindowRectEx(&rcWindow,GetWindowLong(hWnd,GWL_STYLE),GetMenu(hWnd) != 0,GetWindowLong(hWnd,GWL_EXSTYLE));
			rcWindow.right = rcWindow.right - rcWindow.left;
			rcWindow.bottom = rcWindow.bottom - rcWindow.top;
			rcWindow.left=(int)((int)dwScreenWidth-(int)rcWindow.right)/2;
			rcWindow.top=(int)((int)dwScreenHeight-(int)rcWindow.bottom)/2;
			rcWindow.right+=rcWindow.left;
			rcWindow.bottom+=rcWindow.top;
			
			MoveWindow(hWnd,rcWindow.left,rcWindow.top,rcWindow.right-rcWindow.left,rcWindow.bottom-rcWindow.top,TRUE);
			ShowWindow(hWnd,SW_SHOWNORMAL);	
			UpdateWindow(hWnd);

			m_bWndMode = TRUE;
			bOk = TRUE;
		}
	}
	return m_bWndMode;
}

}
#endif