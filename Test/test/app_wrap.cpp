


#include "Common.h"

#if (TARGET_PLATFORM == PLATFORM_WIN32)
#include <Windows.h>
#endif
#include <assert.h>

using namespace xs;
#include "Base.h"
#include "Re.h"
#include "IRenderSystem.h"
#include "IRenderEngine.h"

#include "GlobalClient.h"

#include "StringHelper.h"
#include "Thread.h"
#include "FileSystem/Stream.h"
#include "FileSystem/MemStream.h"
#include "FileSystem/IFileSystem.h"
#include "FileSystem/VirtualFileSystem.h"


#include "app_wrap.h"



//namespace xs
//{
//	extern "C" __declspec(dllexport) IRenderSystem* createRenderSystem(RenderEngineCreationParameters* param);
//}

using namespace xs;

IRenderSystem* g_psRenderSystem = 0;
static GlobalClient gs_global;

bool AppWrap::init(void* hwnd)
{
	RenderEngineCreationParameters param;
	param.rst = RS_OPENGLES2;
	param.fullscreen;	
	param.stencilBuffer;
	param.refreshRate = 60;
	param.colorDepth = 32;
	param.hwnd = hwnd;

	//g_psRenderSystem = xs::createRenderSystem(&param);
    if(!gs_global.create(hwnd))
    {
        return false;
    }
    
    g_psRenderSystem = gs_global.getRenderSystem();
    
	if(!g_psRenderSystem) return false;

	return true;
}

void AppWrap::update(int delta_ms)
{
	g_psRenderSystem->setClearColor(ColorValue(1.0,0,0,1.0));
	g_psRenderSystem->beginFrame(true,true,true);

	g_psRenderSystem->endFrame();
}

void AppWrap::uninit()
{
	//g_psRenderSystem->release();
    gs_global.close();
}

