#include "TxGuiReRenderer.h"
#include "TxSysGui.h"
#include "TxGuiReResourceProvider.h"
#include "TxGuiReTexture.h"
#include "stringhelper\StringConv.h"
#include <vector>

namespace xs{

/*************************************************************************
Constants definitions
*************************************************************************/
const int TxGuiRenderer::VERTEX_PER_QUAD			= 6;
const int TxGuiRenderer::VERTEX_PER_TRIANGLE		= 3;
const int TxGuiRenderer::VERTEXBUFFER_CAPACITY		= RENDERER_VBUFF_CAPACITY;

TxGuiRenderer::TxGuiRenderer(IRenderSystem*	pRenderSystem) : m_pResourceProvider(0),d_currTexture(0),d_bufferPos(0),m_rendererPositionX(0)
{
	m_vBatches.reserve(1024);

	m_pRenderSystem = pRenderSystem;
	d_identifierString = "TxGuiRenderer";

	d_maxTextureSize = m_pRenderSystem->getCapabilities()->getMaxTextureSize();
	updateDisplayArea();

	m_pVB = m_pRenderSystem->getBufferManager()->createVertexBuffer(24,
		RENDERER_VBUFF_CAPACITY,
		BU_DYNAMIC_WRITE_ONLY);

	m_pVBQuick = m_pRenderSystem->getBufferManager()->createVertexBuffer(24,
		VERTEX_PER_QUAD,
		BU_DYNAMIC_WRITE_ONLY);

	m_alphaMaskImages.clear();

	m_renderSystemType = m_pRenderSystem->getRenderSystemType();
}

TxGuiRenderer::~TxGuiRenderer()
{
	safeRelease(m_pVB);
	safeRelease(m_pVBQuick);
	safeDelete(m_pResourceProvider);
	m_alphaMaskImages.clear();
}

xsgui::ResourceProvider* TxGuiRenderer::createResourceProvider(void )
{
	m_pResourceProvider = new TxGuiResourceProvider;
	return m_pResourceProvider;
}

/*************************************************************************
convert colour value to whatever the OpenGL system / D3D9 system is expecting.
*************************************************************************/
RGBA TxGuiRenderer::colourToRe(const xsgui::colour& col) const
{
	ColorValue color(col.getRed(),col.getGreen(),col.getBlue(),col.getAlpha());
	if( m_renderSystemType == RS_D3D9 ) // direct3d
		return color.getAsARGB();
	else								//opengl
		return color.getAsABGR();
}

void  TxGuiRenderer::updateDisplayArea()
{
	int left,top,width,height;
	
	static int s_width = 0 , s_height = 0;
	m_pRenderSystem->getViewport(left,top,width,height);
	d_viewportRect.d_left = left;
	d_viewportRect.d_top = top;
	d_viewportRect.d_right = left + width;
	d_viewportRect.d_bottom = top + height;

	//
	//int w;
	////if(s_width != width || s_height != height || true)
	//	{
	//		s_width = width;
	//		s_height = height;
	//		w = height / 0.75;
	//		if ( w < width )
	//		{	
	//			left = (width - w)/2;
	//			m_rendererPositionX = (float)left / (float)width;
	//			width = w;		
	//		}
	//		else
	//		{
	//			m_rendererPositionX = 0.0;
	//		}
	//	}	

	////commented by xxh
	////这样设置会导致viewport和最上层ui的窗口不吻合，导致某些情况下ui绘制到viewport外面
	//int nW,nH;
	//if(width >  height + 0.3333333f * height)
	//{
	//	nH = width * 0.75f;
	//	nW = width;
	//}
	//else
	//{
	//	nW = height * 1.3333333f;
	//	nH = height;
	//}

	d_display_area.d_left = left;
	d_display_area.d_top = top;
	//modified by xxh
	//d_display_area.d_bottom = top + nH;
	//d_display_area.d_right = left + nW;	
	d_display_area.d_bottom = top + height ;
	d_display_area.d_right = left + width;
}

/*************************************************************************
render a quad directly to the display
*************************************************************************/
void  TxGuiRenderer::renderQuadDirect(const xsgui::Rect& dest_rect, float z, const xsgui::Texture* tex, const xsgui::Rect& texture_rect, const xsgui::ColourRect& colours,xsgui::QuadSplitMode quad_split_mode)
{
	updateDisplayArea();
	QuadInfo quad;
	quad.position = dest_rect;
	/*quad.position.d_left	= dest_rect.d_left;
	quad.position.d_right	= dest_rect.d_right;
	quad.position.d_bottom	= d_display_area.d_bottom - dest_rect.d_bottom;
	quad.position.d_top		= d_display_area.d_bottom - dest_rect.d_top;*/
	quad.texPosition		= texture_rect;

	quad.topLeftCol		= colourToRe(colours.d_top_left);
	quad.topRightCol	= colourToRe(colours.d_top_right);
	quad.bottomLeftCol	= colourToRe(colours.d_bottom_left);
	quad.bottomRightCol	= colourToRe(colours.d_bottom_right);

	MyQuad *myquad = (MyQuad*)m_pVBQuick->lock(0,0,BL_DISCARD);

	//vert0
	myquad[0].vertex[0] = quad.position.d_left;
	myquad[0].vertex[1] = quad.position.d_top;
	myquad[0].vertex[2] = z;
	myquad[0].color     = quad.topLeftCol;
	myquad[0].tex[0]    = quad.texPosition.d_left;
	myquad[0].tex[1]    = quad.texPosition.d_top;

	//vert1
	myquad[1].vertex[0] = quad.position.d_left;
	myquad[1].vertex[1] = quad.position.d_bottom;
	myquad[1].vertex[2] = z;
	myquad[1].color     = quad.bottomLeftCol;     
	myquad[1].tex[0]    = quad.texPosition.d_left;
	myquad[1].tex[1]    = quad.texPosition.d_bottom;

	//vert2

	// top-left to bottom-right diagonal
	if (quad_split_mode == xsgui::TopLeftToBottomRight)
	{
		myquad[2].vertex[0] = quad.position.d_right;
		myquad[2].vertex[1] = quad.position.d_bottom;
		myquad[2].vertex[2] = z;
		myquad[2].color     = quad.bottomRightCol;
		myquad[2].tex[0]    = quad.texPosition.d_right;
		myquad[2].tex[1]    = quad.texPosition.d_bottom;
	}
	// bottom-left to top-right diagonal
	else
	{
		myquad[2].vertex[0] = quad.position.d_right;
		myquad[2].vertex[1] = quad.position.d_top;
		myquad[2].vertex[2] = z;
		myquad[2].color     = quad.topRightCol;
		myquad[2].tex[0]    = quad.texPosition.d_right;
		myquad[2].tex[1]    = quad.texPosition.d_top;
	}

	//vert3
	myquad[3].vertex[0] = quad.position.d_right;
	myquad[3].vertex[1] = quad.position.d_top;
	myquad[3].vertex[2] = z;
	myquad[3].color     = quad.topRightCol;      
	myquad[3].tex[0]    = quad.texPosition.d_right;
	myquad[3].tex[1]    = quad.texPosition.d_top;

	//vert4

	// top-left to bottom-right diagonal
	if (quad_split_mode == xsgui::TopLeftToBottomRight)
	{
		myquad[4].vertex[0] = quad.position.d_left;
		myquad[4].vertex[1] = quad.position.d_top;
		myquad[4].vertex[2] = z;
		myquad[4].color     = quad.topLeftCol;
		myquad[4].tex[0]    = quad.texPosition.d_left;
		myquad[4].tex[1]    = quad.texPosition.d_top;
	}
	// bottom-left to top-right diagonal
	else
	{
		myquad[4].vertex[0] = quad.position.d_left;
		myquad[4].vertex[1] = quad.position.d_bottom;
		myquad[4].vertex[2] = z;
		myquad[4].color     = quad.bottomLeftCol;
		myquad[4].tex[0]    = quad.texPosition.d_left;
		myquad[4].tex[1]    = quad.texPosition.d_bottom;
	}

	//vert5
	myquad[5].vertex[0] = quad.position.d_right;
	myquad[5].vertex[1] = quad.position.d_bottom;
	myquad[5].vertex[2] = z;
	myquad[5].color     = quad.bottomRightCol;
	myquad[5].tex[0]    = quad.texPosition.d_right;
	myquad[5].tex[1]    = quad.texPosition.d_bottom;

	//判断是否要移动x,y坐标
	if( m_renderSystemType == RS_D3D9)
	{
		for( uint i = 0; i < 6; ++i)
		{
			myquad[i].vertex[0] -= 0.5f;
			myquad[i].vertex[1] -= 0.5f;
		}
	}

	m_pVBQuick->unlock();
	std::vector<RenderBatch> v;
	RenderBatch rb;
	rb.pTexture = ((TxGuiTexture*)tex)->m_pTexture;
	rb.ui32VertexStart = 0;
	rb.ui32NumVertices = 6;
	v.push_back(rb);


	m_pRenderSystem->setDiffuseVertexBuffer(m_pVBQuick,8);
	m_pRenderSystem->setTexcoordVertexBuffer(0,m_pVBQuick,0);
	m_pRenderSystem->setVertexVertexBuffer(m_pVBQuick,12);
	m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);

	renderBatches(v);

	m_pRenderSystem->setVertexVertexBuffer(0);
	m_pRenderSystem->setDiffuseVertexBuffer(0);
	m_pRenderSystem->setTexcoordVertexBuffer(0,0);
}

void  TxGuiRenderer::addQuadDirect(const xsgui::Rect& dest_rect, float z, const xsgui::Texture* tex, const xsgui::Rect& texture_rect, const xsgui::ColourRect& colours,xsgui::QuadSplitMode quad_split_mode)
{
	updateDisplayArea();
	renderQuadDirect(dest_rect, z, tex, texture_rect, colours, quad_split_mode);
}

void  TxGuiRenderer::addQuad(const xsgui::Rect& dest_rect, float z, const xsgui::Texture* tex, const xsgui::Rect& texture_rect, const xsgui::ColourRect& colours,xsgui::QuadSplitMode quad_split_mode)
{
	updateDisplayArea();
	// if not queuing, render directly (as in, right now!)
	if (!d_queueing)
	{
		renderQuadDirect(dest_rect, z, tex, texture_rect, colours, quad_split_mode);
	}
	else
	{
		QuadInfo quad;
		quad.position			= dest_rect;
		/*quad.position.d_bottom	= d_display_area.d_bottom - dest_rect.d_bottom;
		quad.position.d_top		= d_display_area.d_bottom - dest_rect.d_top;*/
		quad.z					= z;
		quad.pTexture			= ((TxGuiTexture*)tex)->m_pTexture;
		quad.texPosition		= texture_rect;
		quad.topLeftCol		= colourToRe(colours.d_top_left);
		quad.topRightCol	= colourToRe(colours.d_top_right);
		quad.bottomLeftCol	= colourToRe(colours.d_bottom_left);
		quad.bottomRightCol	= colourToRe(colours.d_bottom_right);

		// set quad split mode
		quad.splitMode = quad_split_mode;

		d_quadlist.insert(quad);
	}

}

void  TxGuiRenderer::renderBatches(std::vector<RenderBatch>& v)
{
	//默认是(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA)
	SceneBlendFactor src,dst;
	m_pRenderSystem->getSceneBlending(src,dst);
	m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
	string blend1plus1("blend:1+1");
	string blendaplus1("blend:a+1");
	string dummy("");

	PP_BY_NAME("TxGuiRenderer::renderBatches");
	uint size = v.size();
	for(uint i = 0;i < size;i++)
	{
		if( v[i].pTexture && v[i].pTexture->getCustomInfo(blend1plus1,dummy) )
			m_pRenderSystem->setSceneBlending(SBF_ONE,SBF_ONE);
		else if( v[i].pTexture && v[i].pTexture->getCustomInfo(blendaplus1,dummy) )
			m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE);
		else
			m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);

		PP_BY_NAME_START("TxGuiRenderer::renderBatches::m_pRenderSystem->setTexture");
		m_pRenderSystem->setTexture(0,v[i].pTexture);
		PP_BY_NAME_STOP();
		PP_BY_NAME_START("TxGuiRenderer::renderBatches::m_pRenderSystem->drawPrimitive");
		m_pRenderSystem->drawPrimitive(PT_TRIANGLES,v[i].ui32VertexStart,v[i].ui32NumVertices);
		PP_BY_NAME_STOP();
	}
	d_bufferPos = 0;
	d_vertex_start = 0;
	v.clear();

	//恢复场景混合
	m_pRenderSystem->setSceneBlending(src,dst);
}
void  TxGuiRenderer::doRender()
{
	PP_BY_NAME("TxGuiRenderer::doRender");
	m_pRenderSystem->setTexcoordVertexBuffer(0,m_pVB,0);
	m_pRenderSystem->setDiffuseVertexBuffer(m_pVB,8);
	m_pRenderSystem->setVertexVertexBuffer(m_pVB,12);
	bool bDepthBufferCheckEnabled = m_pRenderSystem->isDepthBufferCheckEnabled();

	bool bDepthBufferWriteEnabled = m_pRenderSystem->isDepthBufferWriteEnabled();

	SceneBlendFactor src,dst;
	m_pRenderSystem->getSceneBlending(src,dst);
	m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);

	CompareFunction depthBufferFunc = m_pRenderSystem->getDepthBufferFunction();
	//m_pRenderSystem->setDepthBufferFunction(CMPF_LESS);
	m_pRenderSystem->setDepthBufferCheckEnabled(false);
	m_pRenderSystem->setDepthBufferWriteEnabled(true);		

	//判断是否要移动x,y坐标
	float coordinateShift = 0.0f;
	if( m_renderSystemType == RS_D3D9 ) coordinateShift = -0.5f;

	m_vBatches.clear();
	d_vertex_start = 0;
	d_currTexture = 0;
	d_bufferPos = 0;

	MyQuad *myBuff = (MyQuad*)m_pVB->lock(0,0,BL_DISCARD);

	// iterate over each quad in the list
	for (QuadList::iterator i = d_quadlist.begin(); i != d_quadlist.end(); ++i)
	{
		const QuadInfo& quad = (*i);

		if(d_currTexture != quad.pTexture)
		{
			if(d_bufferPos)
			{
				RenderBatch rb;
				rb.pTexture = d_currTexture;
				rb.ui32VertexStart = d_vertex_start;
				rb.ui32NumVertices = d_bufferPos - d_vertex_start;
				m_vBatches.push_back(rb);

				d_vertex_start = d_bufferPos;
			}
			d_currTexture = quad.pTexture;
		}

		//vert0       
		myBuff[d_bufferPos].vertex[0]	= quad.position.d_left + coordinateShift;
		myBuff[d_bufferPos].vertex[1]	= quad.position.d_top + coordinateShift;
		myBuff[d_bufferPos].vertex[2]	= quad.z;
		myBuff[d_bufferPos].color		= quad.topLeftCol;
		myBuff[d_bufferPos].tex[0]		= quad.texPosition.d_left;
		myBuff[d_bufferPos].tex[1]		= quad.texPosition.d_top;
		++d_bufferPos;

		//vert1
		myBuff[d_bufferPos].vertex[0]	= quad.position.d_left + coordinateShift;
		myBuff[d_bufferPos].vertex[1]	= quad.position.d_bottom + coordinateShift;
		myBuff[d_bufferPos].vertex[2]	= quad.z;
		myBuff[d_bufferPos].color		= quad.bottomLeftCol;
		myBuff[d_bufferPos].tex[0]		= quad.texPosition.d_left;
		myBuff[d_bufferPos].tex[1]		= quad.texPosition.d_bottom;
		++d_bufferPos;

		//vert2

		// top-left to bottom-right diagonal
		if (quad.splitMode == xsgui::TopLeftToBottomRight)
		{
			myBuff[d_bufferPos].vertex[0]	= quad.position.d_right + coordinateShift ;
			myBuff[d_bufferPos].vertex[1]	= quad.position.d_bottom + coordinateShift ;
			myBuff[d_bufferPos].vertex[2]	= quad.z;
			myBuff[d_bufferPos].color		= quad.bottomRightCol;
			myBuff[d_bufferPos].tex[0]		= quad.texPosition.d_right;
			myBuff[d_bufferPos].tex[1]		= quad.texPosition.d_bottom;         
		}
		// bottom-left to top-right diagonal
		else
		{
			myBuff[d_bufferPos].vertex[0]	= quad.position.d_right + coordinateShift;
			myBuff[d_bufferPos].vertex[1]	= quad.position.d_top + coordinateShift;
			myBuff[d_bufferPos].vertex[2]	= quad.z;
			myBuff[d_bufferPos].color		= quad.topRightCol;
			myBuff[d_bufferPos].tex[0]		= quad.texPosition.d_right;
			myBuff[d_bufferPos].tex[1]		= quad.texPosition.d_top;         
		}
		++d_bufferPos;

		//vert3
		myBuff[d_bufferPos].vertex[0]	= quad.position.d_right + coordinateShift;
		myBuff[d_bufferPos].vertex[1]	= quad.position.d_top + coordinateShift;
		myBuff[d_bufferPos].vertex[2]	= quad.z;
		myBuff[d_bufferPos].color		= quad.topRightCol;
		myBuff[d_bufferPos].tex[0]		= quad.texPosition.d_right;
		myBuff[d_bufferPos].tex[1]		= quad.texPosition.d_top;
		++d_bufferPos;

		//vert4

		// top-left to bottom-right diagonal
		if (quad.splitMode == xsgui::TopLeftToBottomRight)
		{
			myBuff[d_bufferPos].vertex[0]	= quad.position.d_left + coordinateShift;
			myBuff[d_bufferPos].vertex[1]	= quad.position.d_top + coordinateShift;
			myBuff[d_bufferPos].vertex[2]	= quad.z;
			myBuff[d_bufferPos].color		= quad.topLeftCol;
			myBuff[d_bufferPos].tex[0]		= quad.texPosition.d_left;
			myBuff[d_bufferPos].tex[1]		= quad.texPosition.d_top;         
		}
		// bottom-left to top-right diagonal
		else
		{
			myBuff[d_bufferPos].vertex[0]	= quad.position.d_left + coordinateShift;
			myBuff[d_bufferPos].vertex[1]	= quad.position.d_bottom + coordinateShift;
			myBuff[d_bufferPos].vertex[2]	= quad.z;
			myBuff[d_bufferPos].color		= quad.bottomLeftCol;
			myBuff[d_bufferPos].tex[0]		= quad.texPosition.d_left;
			myBuff[d_bufferPos].tex[1]		= quad.texPosition.d_bottom;         
		}
		++d_bufferPos;

		//vert 5
		myBuff[d_bufferPos].vertex[0]	= quad.position.d_right + coordinateShift;
		myBuff[d_bufferPos].vertex[1]	= quad.position.d_bottom + coordinateShift;
		myBuff[d_bufferPos].vertex[2]	= quad.z;
		myBuff[d_bufferPos].color		= quad.bottomRightCol;
		myBuff[d_bufferPos].tex[0]		= quad.texPosition.d_right;
		myBuff[d_bufferPos].tex[1]		= quad.texPosition.d_bottom;         
		++d_bufferPos;

		if(d_bufferPos > (VERTEXBUFFER_CAPACITY - VERTEX_PER_QUAD))
		{
			RenderBatch rb;
			rb.pTexture = d_currTexture;
			rb.ui32VertexStart = d_vertex_start;
			rb.ui32NumVertices = d_bufferPos - d_vertex_start;
			m_vBatches.push_back(rb);

			m_pVB->unlock();
			renderBatches(m_vBatches);

			++i;
			if(i != d_quadlist.end())
			{
				myBuff = (MyQuad*)m_pVB->lock(0,0,BL_DISCARD);
				--i;
			}
			else break;
		}
	}
	
	if(d_bufferPos > (int)d_vertex_start)
	{
		RenderBatch rb;
		rb.pTexture = d_currTexture;
		rb.ui32VertexStart = d_vertex_start;
		rb.ui32NumVertices = d_bufferPos - d_vertex_start;
		m_vBatches.push_back(rb);
	}

	d_vertex_start = d_bufferPos;

	if(m_pVB->isLocked())m_pVB->unlock();

	renderBatches(m_vBatches);

	m_pRenderSystem->setDepthBufferFunction(depthBufferFunc);
	m_pRenderSystem->setSceneBlending(src,dst);
	m_pRenderSystem->setDepthBufferCheckEnabled(bDepthBufferCheckEnabled);
	m_pRenderSystem->setDepthBufferWriteEnabled(bDepthBufferWriteEnabled);

	m_pRenderSystem->setVertexVertexBuffer(0);
	m_pRenderSystem->setDiffuseVertexBuffer(0);
	m_pRenderSystem->setTexcoordVertexBuffer(0,0);
	m_pRenderSystem->setTexture(0,0);
}
void  TxGuiRenderer::clearRenderList()
{
	d_quadlist.clear();
}

void  TxGuiRenderer::setQueueingEnabled(bool setting)
{
	d_queueing = setting;
}

xsgui::Texture*	TxGuiRenderer::createTexture()
{
	return new TxGuiTexture(this,m_pRenderSystem);
}

xsgui::Texture*	TxGuiRenderer::createTexture(const xsgui::String& filename, const xsgui::String& resourceGroup)
{
	xsgui::Texture* pTexture = createTexture();
	if( pTexture )
	{
		if(  pTexture->loadFromFile( filename,resourceGroup ) ) 
		{
			return pTexture;
		}
		else 
		{
			static_cast<TxGuiTexture *>(pTexture)->destroyTexture();
			return 0;
		}
	}

	return 0;
}

int TxGuiRenderer::setMaskImage(const xsgui::String& filename, const xsgui::String& resourceGroup) 
{
	int id = -1;
	char *pChar = xsgui::StringConv::Utf8ToChar((char*)filename.c_str());	
	std::string str(pChar);
	xsgui::StringConv::DeletePtr(pChar);
	Image img;
	if(img.load(str))
	{
		m_alphaMaskImages.push_back(img);
		id =  m_alphaMaskImages.size() -1;
	}	
	return id;
}
bool TxGuiRenderer::getMaskState(float x,float y,int id)
{	
	if( x < 0 || y < 0 || id < 0 ||
		(xsgui::uint)id >= m_alphaMaskImages.size() ||
		x >= m_alphaMaskImages[id].getWidth() ||
		y >= m_alphaMaskImages[id].getHeight()
		)
		return false;
	ColorValue c = m_alphaMaskImages[id].getColorAt(x,y,0);
	if((uchar)c.a != 0)
		return false;
	return true;
}
xsgui::Texture*	TxGuiRenderer::createTexture(float size,xsgui::Texture::PixelFormat pf, bool immediate )
{
	return new TxGuiTexture(this,m_pRenderSystem,size,pf, immediate);
}

void  TxGuiRenderer::	destroyTexture(xsgui::Texture* texture)
{
	if(texture)static_cast<TxGuiTexture*>(texture)->destroyTexture();
}

void  TxGuiRenderer::	destroyAllTextures()
{
}

bool	TxGuiRenderer::isQueueingEnabled() const
{
	return d_queueing;
}

float	TxGuiRenderer::getWidth() const
{
	return d_display_area.getWidth();
}

float	TxGuiRenderer::getHeight() const
{
	return d_display_area.getHeight();
}

xsgui::Size	TxGuiRenderer::getSize() const
{
	const_cast<TxGuiRenderer*>(this)->updateDisplayArea();
	return d_display_area.getSize();
}

float TxGuiRenderer::getRendererPositionX(void) const
{
	return m_rendererPositionX;
}

xsgui::Rect TxGuiRenderer::getViewportRect() const
{
	const_cast<TxGuiRenderer*>(this)->updateDisplayArea();
	return d_viewportRect;
}

xsgui::Rect	TxGuiRenderer::getRect() const
{
	return d_display_area;
}

uint	TxGuiRenderer::getMaxTextureSize() const
{
	return d_maxTextureSize;
}

uint	TxGuiRenderer::getHorzScreenDPI() const
{
	return 96;
}

uint	TxGuiRenderer::getVertScreenDPI() const
{
	return 96;
}

}