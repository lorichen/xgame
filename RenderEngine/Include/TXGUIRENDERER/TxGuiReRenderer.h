#ifndef __TxGuiReRenderer_H__
#define __TxGuiReRenderer_H__

#	if defined(TxGuiRenderer_EXPORTS)
#		define TxGuiRendererAPI __declspec(dllexport)
#	else
#		define TxGuiRendererAPI __declspec(dllimport)
#		ifdef _DEBUG
#			pragma comment(lib, "xs_gre_d.lib")
#		elif RELEASEDEBUG
#			pragma comment(lib, "xs_gre_rd.lib")
#		else
#			pragma comment(lib, "xs_gre.lib")
#		endif
#	endif

#include "Base.h"
#define new RKT_NEW
#include "TxSysGui.h"
#include "TimerHandler.h"
#include "Re.h"

//#include "TxSysGuiBase.h"
#include "TxSysGuiRenderer.h"
#include "TxSysGuiTexture.h"

#include <list>
#include <set>
#include <vector>

#define RENDERER_VBUFF_CAPACITY	(2048 * 3)

namespace xs{

class TxGuiRendererAPI TxGuiRenderer : public xsgui::Renderer
{
private:
	IRenderSystem*			m_pRenderSystem;
	xsgui::ResourceProvider*		m_pResourceProvider;
	IVertexBuffer*			m_pVB;
	IVertexBuffer*			m_pVBQuick;	
	
	std::vector<Image> m_alphaMaskImages;	

	float					m_rendererPositionX;

	//处理屏幕像素和纹理像素对齐
	//如果渲染系统是D3D9，则屏幕的x，y坐标都减少0.5
	//如果是OGL，则屏幕坐标不变
	RenderSystemType		m_renderSystemType;



public:
	TxGuiRenderer(IRenderSystem*	pRenderSystem);
	virtual ~TxGuiRenderer();
public:
	/*************************************************************************
	Abstract interface methods
	*************************************************************************/
	/*!
	\brief
	Add a quad to the rendering queue.  All clipping and other adjustments should have been made prior to calling this.

	\param dest_rect
	xsgui::Rect object describing the destination area (values are in pixels)

	\param z
	float value specifying the z co-ordinate / z order of the quad

	\param tex
	pointer to the Texture object that holds the imagery to be rendered

	\param texture_rect
	xsgui::Rect object holding the area of \a tex that is to be rendered (values are in texture co-ordinates).

	\param colours
	ColourRect object describing the Color values that are to be applied when rendering.

	\param quam_split_mode
	One of the QuadSplitMode values specifying the way quads are split into triangles

	\return
	Nothing
	*/
	virtual	void 	addQuad(const xsgui::Rect& dest_rect, float z, const xsgui::Texture* tex, const xsgui::Rect& texture_rect, const xsgui::ColourRect& colours, xsgui::QuadSplitMode quam_split_mode) ;

	virtual void  addQuadDirect(const xsgui::Rect& dest_rect, float z, const xsgui::Texture* tex, const xsgui::Rect& texture_rect, const xsgui::ColourRect& colours,xsgui::QuadSplitMode quad_split_mode);

	/*!
	\brief
	Perform final rendering for all quads that have been queued for rendering

	The contents of the rendering queue is retained and can be rendered again as required.  If the contents is not required call clearRenderList().

	\return
	Nothing
	*/
	virtual	void 	doRender() ;


	/*!
	\brief
	Clears all queued quads from the render queue.

	\return
	Nothing
	*/
	virtual	void 	clearRenderList() ;


	/*!
	\brief
	Enable or disable the queueing of quads from this point on.

	This only affects queueing.  If queueing is turned off, any calls to addQuad will cause the quad to be rendered directly.  Note that
	disabling queueing will not cause currently queued quads to be rendered, nor is the queue cleared - at any time the queue can still
	be drawn by calling doRender, and the list can be cleared by calling clearRenderList.  Re-enabling the queue causes subsequent quads
	to be added as if queueing had never been disabled.

	\param setting
	true to enable queueing, or false to disable queueing (see notes above).

	\return
	Nothing
	*/
	virtual void 	setQueueingEnabled(bool setting) ;


	/*!
	\brief
	Creates a 'null' Texture object.

	\return
	a newly created Texture object.  The returned Texture object has no size or imagery associated with it, and is
	generally of little or no use.
	*/
	virtual	xsgui::Texture*	createTexture() ;


	/*!
	\brief
	Create a Texture object using the given image file.

	\param filename
	xsgui::String object that specifies the path and filename of the image file to use when creating the texture.

	\param resourceGroup
	Resource group identifier to be passed to the resource provider when loading the texture file.

	\return
	a newly created Texture object.  The initial contents of the texture memory is the requested image file.

	\note
	Textures are always created with a size that is a power of 2.  If the file you specify is of a size that is not
	a power of two, the final size will be rounded up.  Additionally, textures are always square, so the ultimate
	size is governed by the larger of the width and height of the specified file.  You can check the ultimate sizes
	by querying the texture after creation.
	*/
	virtual	xsgui::Texture*	createTexture(const xsgui::String& filename, const xsgui::String& resourceGroup) ;


	/*!
	\brief
	Create a Texture object with the given pixel dimensions as specified by \a size.  NB: Textures are always square.

	\param size
	float value that specifies the size to use for the width and height when creating the new texture.

	\param immediate
	bool value that specifies whether create texture immdiately ingnore rendering loop;

	\return
	a newly created Texture object.  The initial contents of the texture memory is undefined / random.

	\note
	Textures are always created with a size that is a power of 2.  If you specify a size that is not a power of two, the final
	size will be rounded up.  So if you specify a size of 1024, the texture will be (1024 x 1024), however, if you specify a size
	of 1025, the texture will be (2048 x 2048).  You can check the ultimate size by querying the texture after creation.
	*/	
	virtual	xsgui::Texture	*	createTexture(float size,xsgui::Texture::PixelFormat pf = xsgui::Texture::PF_RGBA, bool immediate = false);


	/*!
	\brief
	Destroy the given Texture object.

	\param texture
	pointer to the Texture object to be destroyed

	\return
	Nothing
	*/
	virtual	void 		destroyTexture(xsgui::Texture* texture) ;


	/*!
	\brief
	Destroy all Texture objects.

	\return
	Nothing
	*/
	virtual void 		destroyAllTextures() ;


	/*!
	\brief
	Return whether queueing is enabled.

	\return
	true if queueing is enabled, false if queueing is disabled.
	*/
	virtual bool	isQueueingEnabled() const ;


	/*!
	\brief
	Return the current width of the display in pixels

	\return
	float value equal to the current width of the display in pixels.
	*/
	virtual float	getWidth() const	;


	/*!
	\brief
	Return the current height of the display in pixels

	\return
	float value equal to the current height of the display in pixels.
	*/
	virtual float	getHeight() const	;


	/*!
	\brief
	Return the size of the display in pixels

	\return
	Size object describing the dimensions of the current display.
	*/
	virtual xsgui::Size	getSize() const		;


	/*!
	\brief
	get render xpos, so change the window xposition.
	\return
	relative value according to viewport
	*/
	virtual float	getRendererPositionX(void) const;

	/*!
	\brief
	Return a xsgui::Rect describing the screen

	\return
	A xsgui::Rect object that describes the screen area.  Typically, the top-left values are always 0, and the size of the area described is
	equal to the screen resolution.
	*/
	virtual xsgui::Rect	getRect() const		;

	virtual xsgui::Rect getViewportRect() const;

	/*!
	\brief
	Return the maximum texture size available

	\return
	Size of the maximum supported texture in pixels (textures are always assumed to be square)
	*/
	virtual	uint	getMaxTextureSize() const	;


	/*!
	\brief
	Return the horizontal display resolution dpi

	\return
	horizontal resolution of the display in dpi.
	*/
	virtual	uint	getHorzScreenDPI() const	;


	/*!
	\brief
	Return the vertical display resolution dpi

	\return
	vertical resolution of the display in dpi.
	*/
	virtual	uint	getVertScreenDPI() const	;


	/*!
	\brief
		Create transparen mask image for imageset.
	\param filename
		xsgui::String object that specifies the path and filename of the image file to use when creating the texture.

	\param resourceGroup
		Resource group identifier to be passed to the resource provider when loading the texture file.

	\return
		id of mask image,set faile then return -1.
	*/
	virtual int setMaskImage(const xsgui::String& filename, const xsgui::String& resourceGroup);

	/*!
	\brief
		get state(transparent/opacity) of  pos(x,y) int image.
	\int id
		MaskImage id.
	\param x
		x coordinate position
	\param y
		y coordinate position.
	\return 
		state of x,y position with bool type
		true  -- transparent
		false -- opacity.
	*/
	virtual bool getMaskState(float x,float y,int id);

	virtual xsgui::ResourceProvider* createResourceProvider(void );
public:
	/************************************************************************
	Implementation Constants
	************************************************************************/
	const static int			VERTEX_PER_QUAD;							//!< number of vertices per quad
	const static int			VERTEX_PER_TRIANGLE;						//!< number of vertices for a triangle
	const static int			VERTEXBUFFER_CAPACITY;						//!< capacity of the allocated vertex buffer

	/*************************************************************************
	Implementation Structs & classes
	*************************************************************************/
	struct MyQuad
	{
		float tex[2];
		long color;
		float vertex[3];
	};

	/*!
	\brief
	structure holding details about a quad to be drawn
	*/
	struct QuadInfo
	{
		ITexture*	pTexture;  
		xsgui::Rect		position;
		float		z;
		xsgui::Rect		texPosition;
		long		topLeftCol;
		long		topRightCol;
		long		bottomLeftCol;
		long		bottomRightCol;

		xsgui::QuadSplitMode   splitMode;

		bool operator<(const QuadInfo& other) const
		{
			// this is intentionally reversed.
			return z > other.z;
		}

	};
	/*************************************************************************
	Implementation Data
	*************************************************************************/
	typedef std::multiset<QuadInfo>		QuadList;
	QuadList d_quadlist;

	xsgui::Rect		d_display_area;

	xsgui::Rect		d_viewportRect;

	bool		d_queueing;			//!< setting for queuing control.
	ITexture*	d_currTexture;		//!< Currently bound texture.
	int			d_bufferPos;		//!< index into buffer where next vertex should be put.

	int       d_maxTextureSize;		//!< Holds maximum supported texture size (in pixels).

	struct RenderBatch
	{
		uint		ui32VertexStart;
		uint		ui32NumVertices;
		ITexture*	pTexture;
	};
	std::vector<RenderBatch>	m_vBatches;
	uint						d_vertex_start;
	void  renderBatches(std::vector<RenderBatch>& v);
private:
	RGBA colourToRe(const xsgui::colour& col) const;
	void  updateDisplayArea();
	void  renderQuadDirect(const xsgui::Rect& dest_rect, float z, const xsgui::Texture* tex, const xsgui::Rect& texture_rect, const xsgui::ColourRect& colours, xsgui::QuadSplitMode quad_split_mode);
};

}
#endif