


#include "Common.h"

#if (TARGET_PLATFORM == PLATFORM_WIN32)
#include <Windows.h>
#endif
#include <assert.h>


#include "Base.h"
#include "Re.h"

using namespace xs;
#include "IRenderSystem.h"
#include "IRenderEngine.h"

#include "StringHelper.h"
#include "Thread.h"
#include "FileSystem/Stream.h"
#include "FileSystem/MemStream.h"
#include "FileSystem/IFileSystem.h"
#include "FileSystem/VirtualFileSystem.h"
#include "ISceneManager2.h"
#include "Entity2DAniPack.h"
#include "shaderdeclare.h"

#include "GlobalClient.h"
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
static Entity2DAniPack*	g_pMpwTest = 0;
static ITexture* gs_pTex = 0;
static ModelNode* g_pModelNode = 0;

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

	xs::getFileSystem()->addFindPath("");
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

	std::string strMapFile = "data/maps/文曲宫.mp";		//"maps/戮战之野.mp";
	std::string strWayFile = "data/maps/文曲宫路点.xml"; //"maps/戮战之野路点.xml";
    //std::string strPicName = "11.dds";
    
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
    //gs_pTex = g_psRenderSystem->getTextureManager()->createTextureFromFile(strPicName.c_str());
    
	IEntityFactory* pEngityFactory = gs_global.getEntityFactory();

	bool bLoadMap = g_psScenemanager->loadScene(strMapFile.c_str(),strWayFile.c_str(),&rc,pEngityFactory,false,&pt);
	if(!bLoadMap)
	{
		assert(0);
		printf("\n加载错误!");
		return false;
	}
	
	
	/*
	g_pMpwTest = new Entity2DAniPack;
	g_pMpwTest->Open(g_psRenderSystem,"data/mpw/出云村/出云村特效遮罩修改/015.mpw");
    */

	/*
	g_pModelNode = new ModelNode;
	//g_pModelNode->setModel("data/Model/test/stand.MZ");
	g_pModelNode->setModel("data/Model/Common/Effect/新手村特效/新手村3/传送门.TX");
	g_pModelNode->setPosition(0,-0.5,-1.5);
	g_pModelNode->setScale(1,1,1);
	g_pModelNode->setVisible(true);
	*/


	return true;
}

void AppWrap::update(int tick,int delta_ms)
{
	g_psRenderSystem->setClearColor(ColorValue(0.0,0.0,0.0,0.0));
	g_psRenderSystem->beginFrame(true,true,true);
    
    
	g_psRenderSystem->switchTo2D();
	g_psRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);

	//g_psRenderSystem->point(xs::Point(100,100),xs::ColorValue(1.0,0,0,1.0));
	//g_psRenderSystem->line(xs::Point(0,0),xs::Point(800,600),ColorValue(1,0,0,1));
    
    xs::Rect rc(0,0,140,140);
	//g_psRenderSystem->rectangle(rc,gs_pTex);

	//---------------------------------------------
	g_psRenderSystem->switchTo3D();//转换到3d模式，这是默认模式，同时其它地方可能修改了这个模式
	g_psRenderSystem->setWorldMatrix(Matrix4::IDENTITY);

	if(g_psScenemanager)
	{
		g_psScenemanager->setupMatrix();
		g_psRenderSystem->setViewMatrix(g_psScenemanager->getViewMatrix());
		g_psRenderSystem->setProjectionMatrix(g_psScenemanager->getProjectionMatrix());

		g_psScenemanager->update(tick,delta_ms,g_psRenderSystem);
		g_psScenemanager->draw(g_psRenderSystem);
	}
	else
	{
		g_psRenderSystem->setViewMatrix(Matrix4::IDENTITY);
		g_psRenderSystem->setProjectionMatrix(Matrix4::IDENTITY);
	}

	//g_psRenderSystem->switchTo3D();

	//测试mpw绘制成功，可绘制图片
	if(g_pMpwTest)
	{
		//g_psRenderSystem->switchTo2D();
		IShaderProgram* pShader = g_psRenderSystem->getShaderProgram(ESP_V3_UV_GC);
		g_psRenderSystem->bindCurrentShaderProgram(pShader,true);
		g_pMpwTest->Draw(1.f);
	}

	//测试绘制模型
	if(g_pModelNode)
	{
		g_pModelNode->update(tick,delta_ms,g_psRenderSystem);
		g_pModelNode->render(g_psRenderSystem,true);
	}

	g_psRenderSystem->endFrame();
}

void AppWrap::uninit()
{
	//g_psRenderSystem->release();
    gs_global.close();
}

void AppWrap::move(float dx,float dy)
{
	if(g_psScenemanager)
	{
		g_psScenemanager->scrollViewport(dx,dy);
	}
    
    if(g_pModelNode)
    {
        g_pModelNode->yaw(3*dx);
        g_pModelNode->pitch(3*dy);
    }
}

