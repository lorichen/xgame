#include "stdafx.h"
#include "ImageLoader.h"

bool ImageLoader::create(const char* szFileDir)
{
	close();

	m_strPath = szFileDir;

	return true;
}

void ImageLoader::close()
{
	std::vector<ImageRect*>::iterator begin = m_vImageRects.begin();
	std::vector<ImageRect*>::iterator end = m_vImageRects.end();
	for(std::vector<ImageRect*>::iterator it = begin;it != end;++it)
	{
		(*it)->finalize();
		delete (*it);
	}
	m_vImageRects.clear();
}

ImageRect*	ImageLoader::load(WORD index)
{
	xs::CStreamHelper pStream = xs::getFileSystem()->open("NoTexture.dds");
	if(!pStream)
	{
		return 0;
	}
	IRenderEngine *pRenderEngine = getRenderEngine();
	IRenderSystem *pRenderSystem = pRenderEngine->getRenderSystem();
	ImageRect *pImageRect = new ImageRect;
	pImageRect->initialize(pRenderSystem,pStream,"dds",false);
	m_vImageRects.push_back(pImageRect);
	return pImageRect;
}