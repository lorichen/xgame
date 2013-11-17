


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
#include "ISceneManager2.h"

#include "app_wrap.h"



//namespace xs
//{
//	extern "C" __declspec(dllexport) IRenderSystem* createRenderSystem(RenderEngineCreationParameters* param);
//}

using namespace xs;
static unsigned int gs_width,gs_height;
static GlobalClient		gs_global;
static IRenderSystem*	g_psRenderSystem = 0;
static ISceneManager2*  g_psScenemanager = 0;

static ITexture* gs_pTex = 0;

void AppWrap::setViewSize(unsigned int width,unsigned int height)
{
	gs_width = width;
	gs_height = height;
}

bool AppWrap::init(void* hwnd)
{
	RenderEngineCreationParameters param;
	param.rst = RS_OPENGLES2;
	param.fullscreen;	
	param.stencilBuffer;
	param.refreshRate = 60;
	param.colorDepth = 32;
	param.hwnd = hwnd;

    ////xs::getFileSystem()->addFindPath(""); //globalclient have done!
	//xs::getFileSystem()->addFindPath("maps");
    xs::getFileSystem()->addFindPath("data");

	//g_psRenderSystem = xs::createRenderSystem(&param);
    if(!gs_global.create(hwnd))
    {
		assert(0);
        return false;
    }
    
    g_psRenderSystem = gs_global.getRenderSystem();
    
	if(!g_psRenderSystem)
	{
		assert(0);
		return false;
	}

	IResourceManager* pResManager = gs_global.getResourceManager();
	g_psScenemanager = gs_global.getSceneManager();
	
	//------------------test-----------------------------------

	
	g_psScenemanager->setRunType(RUN_TYPE_GAME);

	std::string strMapFile = "maps/cyc.mp";
	std::string strWayFile = "maps/������·��.xml";
    std::string strPicName = "11.dds";
    
    xs::CPath pathTest;
    pathTest = "x\\2\\t\\3.3";
    
    /*
    std::ifstream file;
    file.open("test",std::ios::binary|std::ios::in);
    if(file.is_open())
    {
        //file >> strMapFile;
        //file >> strWayFile;
        file >> strPicName;
        file.close();
    }
    else
    {
        assert(0);
    }
     */
    
	
	xs::Point pt;
	xs::Rect  rc(0,0,gs_width,gs_height);

	//gs_pTex = g_psRenderSystem->getTextureManager()->createTextureFromFile("13.png");
    gs_pTex = g_psRenderSystem->getTextureManager()->createTextureFromFile(strPicName.c_str());
    
	bool bLoadMap = g_psScenemanager->loadScene(strMapFile.c_str(),strWayFile.c_str(),&rc,NULL,false,&pt);
	if(!bLoadMap)
	{
		assert(0);
		printf("\n���ش���!");
		return false;
	}
    
	return true;
}

void AppWrap::update(int tick,int delta_ms)
{
	g_psRenderSystem->setClearColor(ColorValue(0.0,0.0,0.0,0.0));
	g_psRenderSystem->beginFrame(true,true,true);
    
    
	g_psRenderSystem->switchTo2D();
	g_psRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);

	//g_psRenderSystem->point(xs::Point(100,100),xs::ColorValue(1.0,0,0,1.0));
	g_psRenderSystem->line(xs::Point(0,0),xs::Point(800,600),ColorValue(1,0,0,1));
    
    xs::Rect rc(0,0,256,256);
	g_psRenderSystem->rectangle(rc,gs_pTex);

	//---------------------------------------------

	if(g_psScenemanager)
	{
		g_psScenemanager->update(tick,delta_ms,g_psRenderSystem);
		g_psScenemanager->draw(g_psRenderSystem);
	}

	g_psRenderSystem->endFrame();
}

void AppWrap::uninit()
{
	//g_psRenderSystem->release();
    gs_global.close();
}

void AppWrap::move(int dx,int dy)
{
	if(g_psScenemanager)
	{
		g_psScenemanager->scrollViewport(dx,dy);
	}
}

