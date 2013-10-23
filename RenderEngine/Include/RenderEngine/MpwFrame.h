#ifndef __MpwFrame_H__
#define __MpwFrame_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	/**MPW的一帧
	*/
	class _RenderEngineExport MpwFrame
	{
	public:

		bool	initialize(IRenderSystem *pRenderSystem,Image& image,bool bSplit = true);
		bool	initialize(IRenderSystem *pRenderSystem,const std::string& strImageFilename,bool bSplit = true);
		bool	initialize(IRenderSystem *pRenderSystem,xs::Stream* pStream,const std::string& strType,bool bSplit = true);

		void 	finalize();
		void 	render(float alpha = 1.0f);

		uint	getWidth(){return m_width;}
		uint	getHeight(){return m_height;}

		/** (x,y)处是否透明
		@param x x坐标
		@param y y坐标
		@return true表示透明，false反之
		*/\
		bool	isTransparent(int x,int y);

		size_t getMemorySize();

	public:
		MpwFrame();
		~MpwFrame();

	private:
		struct MpwFrameSub
		{
			MpwFrameSub()
			{
				vbTexcoord = 0;
				vb = 0;
				texture = 0;
			}

			IVertexBuffer*	vbTexcoord;
			IVertexBuffer*	vb;
			ITexture* texture;

			Vector3 v[4];
			Vector2 t[4];
		};

	private:
		typedef std::vector<MpwFrameSub>	MpwFrameSubList;
		MpwFrameSubList m_vMpwFrameSub;
		IRenderSystem*	m_pRenderSystem;

		uint	m_width,m_height;
		uint	m_next_power_of_two_width,m_next_power_of_two_height;

		uchar*	m_pTransparentData;
		uint	m_transparentBytesPerLine;

		size_t	m_memorySize;

	private:
		bool 	splitImageTo2N(ITextureManager *pTextureManager,const Image& image,MpwFrameSubList& list,const Rect& rc,uint imageWidth,uint imageHeight,uint level,bool bSplit);
		bool createVBs(const Rect& rc,Vector2 texcoords[4],MpwFrameSub& sub);
	};
	/** @} */
}

#endif