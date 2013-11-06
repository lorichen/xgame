#include "stdafx.h"
#include "IResourceManager.h"
#include "ResourceLoaderMpw.h"
#include "ResourceLoaderMz.h"

namespace rkt
{
	extern "C" __declspec(dllexport) void test()
	{
		using namespace rkt;

		rkt::handle h ;
		rkt::IResourceNode *pRes ;
		rkt::IResourceManager *pResourceMgr = 0;
		pResourceMgr  = CreateResourceManagerProc(1);
		pResourceMgr->registerResourceLoader(rkt::typeResourceModel,new ResourceLoaderMz);

		pResourceMgr->registerResourceLoader(rkt::typeResourcePic,new ResourceLoaderMpw);

		h = pResourceMgr->requestResource(".\\Data\\test\\1.mpw",rkt::typeResourcePic);

		pRes = (rkt::IResourceNode*)(rkt::getHandleData(h));

		h = pResourceMgr->requestResource(".\\Data\\test\\1.Mz",rkt::typeResourceModel);

		pRes = (rkt::IResourceNode*)rkt::getHandleData(h);

		
		rkt::Semaphore semaphore;
		while(true)
		{
			semaphore.Wait(100);
			rkt::IResource* res = pRes->getResource();
			if(res)
			{
				pRes = pRes ;
				break;
			}

		}
		pResourceMgr->release();

		/*ResourceLoaderMz *p1 = new ResourceLoaderMz;\
		IResource *pr1 = p1->loadResource("Test\\5.mz");

		ResourceLoaderMpw *p2 = new ResourceLoaderMpw;
		IResource *pr2 = p2->loadResource("Test\\1.mpw");

		pr1->release();
		pr2->release();
		p1->release();
		p2->release();*/
	}
}