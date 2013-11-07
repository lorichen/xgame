// pctest.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <Windows.h>

#include "Common.h"
#include "StringHelper.h"
#include "Thread.h"
#include "FileSystem/Stream.h"
#include "FileSystem/MemStream.h"
#include "FileSystem/IFileSystem.h"
#include "FileSystem/VirtualFileSystem.h"

#include "Re.h"
#include "IRenderSystem.h"
#include "IRenderEngine.h"


#include "../app_wrap.h"
using namespace xs;

#pragma comment(lib, "xs_com_rd.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"RenderSystemGLES2_rd.lib")


LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch(msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case  'w':
		case  'W':
			
			break;

		case 's':
		case 'S':
			

			break;

		case 'a':
		case 'A':
			

			break;

		case 'd':
		case 'D':
			
			break;

		}
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

#define WNDCLASS_NAME "test"
int _tmain(int argc, _TCHAR* argv[])
{

	RenderEngineCreationParameters param;
	param.rst = RS_OPENGLES2;
	param.fullscreen;	
	param.stencilBuffer;
	param.w = 800;
	param.h = 600;
	param.refreshRate = 60;
	param.colorDepth = 32;
	IRenderEngine* pRenderEngine = NULL;
	IRenderSystem* pRender = NULL;

	// Register the window class
	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
		WNDCLASS_NAME, NULL
	};
	RegisterClassEx( &wc );

	// Create window instance
	int nScreenWidth	= param.w;
	int nScreenHeight	= param.h;

	int width			= nScreenWidth + GetSystemMetrics(SM_CXFIXEDFRAME)*2;
	int height			= nScreenHeight + GetSystemMetrics(SM_CYFIXEDFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);

	RECT rectW;
	LONG styleW,styleFS, styleEx;
	RECT rectFS;
	rectW.left		= (GetSystemMetrics(SM_CXSCREEN)-width)/2;
	rectW.top		= (GetSystemMetrics(SM_CYSCREEN)-height)/2;
	rectW.right		= rectW.left+width;
	rectW.bottom	= rectW.top+height;
	styleW			= WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_SIZEBOX; //WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;


	rectFS.left		= 0;
	rectFS.top		= 0;
	rectFS.right	= nScreenWidth;
	rectFS.bottom	= nScreenHeight;
	styleFS			= WS_POPUP|WS_VISIBLE; //WS_POPUP

	styleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;


	HINSTANCE hInstance	= ::GetModuleHandle(0);

	param.hwnd = ::CreateWindowEx(styleEx, WNDCLASS_NAME, WNDCLASS_NAME, styleW,
		rectW.left, rectW.top, rectW.right-rectW.left, rectW.bottom-rectW.top,
		GetDesktopWindow(), NULL, hInstance, NULL);

	RECT wRect;
	GetClientRect((HWND)param.hwnd,&wRect);

	if (param.hwnd )
	{
		// Show the window
		ShowWindow( (HWND)param.hwnd, SW_SHOWDEFAULT );
		UpdateWindow( (HWND)param.hwnd );

		//文件系统初始化，加路径

		//getFileSystem()->addFindPath("data");
		//getFileSystem()->addFindPath("");

		//---------------------------
		bool b = AppWrap::init(param.hwnd);
		assert(b);

		LARGE_INTEGER nFreq;
		LARGE_INTEGER nLast;
		LARGE_INTEGER nNow;

		QueryPerformanceFrequency(&nFreq);
		QueryPerformanceCounter(&nLast);

		// Enter the message loop
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while(msg.message != WM_QUIT)
		{
			if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				// Get current time tick.
				//QueryPerformanceCounter(&nNow);

				static int g_lastTime = timeGetTime();
				int timenow = timeGetTime();

				// If it's the time to draw next frame, draw it, else sleep a while.
				int dt = timenow - g_lastTime;
				if (dt > 20)
				{
					g_lastTime = timenow;
					AppWrap::update(dt);
				}
				else
				{
					Sleep(5);
				}

			}
		}

		AppWrap::uninit();
	}

	return 0;
}

