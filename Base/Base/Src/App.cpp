/**
* Created by pk 2008.01.07
*/

#include "stdafx.h"
#include "App.h"

#define new RKT_NEW

#ifdef RKT_WIN32
#	include <windows.h>
#endif

#define CLASS_NAME					"CAppWindow"
#define APPLICATION_NAME			"新宋演义"
#define REGISTER_CLASS_ERROR		"Failed to register window class"

#ifdef RKT_WIN32
LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		xs::CAppWindow* appWindowPtr = (xs::CAppWindow*)GetWindowLong((HWND)hWnd, GWL_USERDATA);
		if (appWindowPtr)
		{
			xs::CMessageDispatcher* msgDispatcher = appWindowPtr->getMessageDispatcher();
			if (msgDispatcher && msgDispatcher->onMessage(hWnd, message, wParam, lParam))
				return 0;
		}
		return(DefWindowProc((HWND)hWnd, (UINT)message, (WPARAM)wParam, (LPARAM)lParam));
	}

	return 0;
}
#endif



namespace xs {

#ifdef RKT_WIN32

int CAppWin32::run()
{
	if (!onCreate())
		return 0;

	int doIdle = 1;
	int idleCount = 0;
	int ret;
	MSG msg;
	
	for (;;)	
	{
		while (doIdle && !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!onIdle(idleCount++))
				doIdle = 0;
		}

		ret = GetMessage(&msg, NULL, 0, 0);
		if (ret == -1) // error, don't process
			continue;  
		else if (!ret) // WM_QUIT, exit message loop
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		switch (msg.message)
		{
			// These messages should NOT cause idle processing
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
		case WM_PAINT:
		case 0x0118:	// WM_SYSTIMER (caret blink)
			break;
		default:
			//crr add
			//在IGlobalClient.h里定义
//			enum
// 			{
// 				WM_BACK_TO_SELECT_SERVER = WM_USER + 0,
// 				WM_BACK_TO_RE_LOGIN = WM_USER + 1,			//将来策划可能要求转到登录界面
// 				WM_BACK_TO_SELECT_ACTOR = WM_USER + 2,
// 			};
			//为避免耦合，这里直接使用>=  WM_USER作为判断
			if (msg.message >=  WM_USER)
			{
				break;
			}
			doIdle = TRUE;
			idleCount = 0;
			break;
		}
	}
	/*
	MSG msg;
	while(true)	
	{
	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
	if (msg.message == WM_QUIT)
	break;

	if (GetMessage (&msg, NULL, 0, 0))
	{
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	}
	}
	else
	{
	fclDispatchTrace();
	onIdle();
	}
	}*/

	onClose();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// CMessageDispatcher
/// 检测指定的键是否处于按下状态
/// eg: isKeyPressed('A');isKeyPressed('8');isKeyPressed(VK_MENU);
RKT_API bool isKeyPressed(uint virtualKeyCode)
{
	return (GetKeyState(virtualKeyCode) & 0x8000) != 0;
}

//////////////////////////////////////////////////////////////////////////
// CAppWindow
bool CAppWindow::createWindow(ulong w, ulong h, ulong xPos, ulong yPos, bool centerWnd)
{
	WNDCLASS    wndClass;

	wndClass.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC/*| CS_DBLCLKS*/;
	wndClass.lpfnWndProc    = /*(LRESULT (__stdcall*)(HWND, UINT, WPARAM, LPARAM) )*/wndProc;
	wndClass.cbClsExtra     = 0;
	wndClass.cbWndExtra     = 0;
	wndClass.hInstance      = GetModuleHandle(0);
	wndClass.hIcon          = NULL;
	wndClass.hCursor        = LoadCursor(0, IDC_ARROW);
	wndClass.hbrBackground  = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wndClass.lpszMenuName   = 0;
	wndClass.lpszClassName  = CLASS_NAME;

	//modified by xxh for fixed size window
	DWORD wndstyle = /*WS_OVERLAPPEDWINDOW */WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;

	RECT wndrc;
	wndrc.left = 0; wndrc.top = 0; wndrc.right = w; wndrc.bottom = h;
	::AdjustWindowRect(&wndrc, wndstyle, FALSE);

	//在窗口大小大于屏幕大小时，用ulong会出现错误
	//ulong actualWidth = wndrc.right - wndrc.left;
	//ulong actualHeight = wndrc.bottom - wndrc.top;
	//ulong cx = GetSystemMetrics(SM_CXSCREEN);
	//ulong cy = GetSystemMetrics(SM_CYSCREEN);

	int actualWidth = wndrc.right - wndrc.left;
	int actualHeight = wndrc.bottom - wndrc.top;
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	//ulong cx = GetSystemMetrics(SM_CXSCREEN);
	//ulong cy = GetSystemMetrics(SM_CYSCREEN);
	//ulong actualWidth = w + GetSystemMetrics(SM_CXSIZEFRAME) * 2;
	//ulong actualHeight = h + GetSystemMetrics(SM_CYSIZEFRAME) * 2 + /*GetSystemMetrics(SM_CYMENU) +*/ GetSystemMetrics(SM_CYCAPTION);

	//ulong actualPosX = centerWnd ? (cx - actualWidth) / 2 : xPos;
	//ulong actualPosY = centerWnd ? (cy - actualHeight) / 2 : yPos;

	int actualPosX = centerWnd ? (cx - actualWidth) / 2 : xPos;
	int actualPosY = centerWnd ? (cy - actualHeight) / 2 : yPos;

	if (RegisterClass(&wndClass))
	{
		m_hWnd = (void*)CreateWindow(CLASS_NAME, APPLICATION_NAME, wndstyle/*WS_OVERLAPPEDWINDOW*/,
			actualPosX, actualPosY, actualWidth, actualHeight, 0, 0, GetModuleHandle(0), 0);
		if (!m_hWnd)
			return false;
	}
	else
	{
		MessageBox(0, REGISTER_CLASS_ERROR, APPLICATION_NAME, MB_ICONERROR | MB_OK);
		return false;
	}

	SetWindowLong((HWND)m_hWnd, GWL_USERDATA, (LONG)this);

	//ShowWindow((HWND)m_hWnd, SW_NORMAL);
	UpdateWindow((HWND)m_hWnd);

	return true;
}

void CAppWindow::setWindowTitle(const char* title)
{
	if (m_hWnd != NULL && title != NULL)
		SetWindowText((HWND)m_hWnd, title);
}

void CAppWindow::showWindow(bool show)
{
	if( !m_hWnd) return;
	if( show)
		::ShowWindow((HWND)m_hWnd, SW_NORMAL);
	else
		::ShowWindow((HWND)m_hWnd, SW_HIDE);
}

#else

//#error not implement CAppWin32 and CAppWindow

#endif

} // namespace xs
