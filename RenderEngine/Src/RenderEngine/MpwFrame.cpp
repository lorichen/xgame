#include "StdAfx.h"
#include "Image.h"
#include "MpwFrame.h"

namespace xs
{
#define MPW_FRAME_SPLIT_DEPTH	2

	bool MpwFrame::createVBs(const Rect& rc,Vector2 texcoords[4],MpwFrameSub& sub)
	{
		//by yhc 不用加一,会导致有缩放
		/*
		sub.v[0] = Vector3(rc.left,rc.bottom + 1,0);
		sub.v[1] = Vector3(rc.left,rc.top,0);
		sub.v[2] = Vector3(rc.right + 1,rc.top,0);
		sub.v[3] = Vector3(rc.right + 1,rc.bottom + 1,0);*/

		//左上像素准则
		if( m_pRenderSystem->getRenderSystemType() == RS_D3D9 )
		{
			//d3d要偏移0.5个像素
			sub.v[0] = Vector3(rc.left-0.5f,rc.bottom-0.5f ,0);
			sub.v[1] = Vector3(rc.left-0.5f,rc.top-0.5f,0);
			sub.v[2] = Vector3(rc.right-0.5f,rc.top-0.5f,0);
			sub.v[3] = Vector3(rc.right-0.5f,rc.bottom-0.5f,0);
		}
		else
		{
			sub.v[0] = Vector3(rc.left,rc.bottom ,0);
			sub.v[1] = Vector3(rc.left,rc.top,0);
			sub.v[2] = Vector3(rc.right,rc.top,0);
			sub.v[3] = Vector3(rc.right,rc.bottom,0);
		}

		

		sub.t[0] = texcoords[0];
		sub.t[1] = texcoords[1];
		sub.t[2] = texcoords[2];
		sub.t[3] = texcoords[3];

		sub.vb = m_pRenderSystem->getBufferManager()->createVertexBuffer(12,4,BU_STATIC_WRITE_ONLY);
		if(sub.vb)
		{
			Vector3 *p = (Vector3*)sub.vb->lock(0,0,BL_DISCARD);
			if(p)
			{
				*p++ = sub.v[0];
				*p++ = sub.v[1];
				*p++ = sub.v[2];
				*p++ = sub.v[3];
				sub.vb->unlock();
			}
		}
		sub.vbTexcoord = m_pRenderSystem->getBufferManager()->createVertexBuffer(8,4,BU_STATIC_WRITE_ONLY);
		if(sub.vbTexcoord)
		{
			Vector2 *p = (Vector2*)sub.vbTexcoord->lock(0,0,BL_DISCARD);
			if(p)
			{
				*p++ = sub.t[0];
				*p++ = sub.t[1];
				*p++ = sub.t[2];
				*p++ = sub.t[3];
				sub.vbTexcoord->unlock();
			}
		}
		return true;
	}

	bool 	MpwFrame::splitImageTo2N(ITextureManager *pTextureManager,const Image& image,MpwFrameSubList& list,const Rect& rc,uint imageWidth,uint imageHeight,uint level,bool bSplit)
	{
		uint nextP2Width = Math::NextP2(rc.right - rc.left + 1);
		uint nextP2Height = Math::NextP2(rc.bottom - rc.top + 1);

		if(m_pRenderSystem->getCapabilities()->hasCapability(RSC_NON_POWER_OF_2_TEXTURES) ||
			(imageWidth == nextP2Width && imageHeight == nextP2Height))
		{
			MpwFrameSub sub;

			Vector2 texcoords[4] = 
			{
				Vector2(0.0f,0.0f),
				Vector2(0.0f,1.0f),
				Vector2(1.0f,1.0f),
				Vector2(1.0f,0.0f)
			};
			createVBs(rc,texcoords,sub);

			sub.texture = pTextureManager->createTextureFromImage(image, 0, FO_POINT, FO_POINT, FO_NONE, TAM_CLAMP_TO_EDGE, TAM_CLAMP_TO_EDGE);
			if( NULL == sub.texture)
				return false;
			list.push_back(sub);
			return true;
		}

		//对于MPW_FRAME_SPLIT_DEPTH == 2的情况，就是限制在分割成的图片最多不超过3 * 3
		if(level >= MPW_FRAME_SPLIT_DEPTH || !bSplit)
		{
			float xRatio = (rc.right - rc.left + 1) / (float)nextP2Width;
			float yRatio = (rc.bottom - rc.top + 1) / (float)nextP2Height;

			Vector2 texcoords[4] = 
			{
				Vector2(0.0f,0.0f),
				Vector2(0.0f,yRatio),
				Vector2(xRatio,yRatio),
				Vector2(xRatio,0.0f)
			};
			MpwFrameSub sub;
			createVBs(rc,texcoords,sub);
			sub.texture = pTextureManager->createTextureFromImage(image,
				&rc,
				nextP2Width,
				nextP2Height,
				FO_POINT, 
				FO_POINT,
				FO_NONE, 
				TAM_CLAMP_TO_EDGE, 
				TAM_CLAMP_TO_EDGE);
			if( NULL == sub.texture)
				return false;
			list.push_back(sub);
			return true;;
		}

		uint lastP2Width = Math::LastP2(rc.right - rc.left + 1);
		uint lastP2Height = Math::LastP2(rc.bottom - rc.top + 1);
		Rect rect;
		rect.left = rc.left;
		rect.top = rc.top;
		rect.right = rect.left + lastP2Width - 1;
		rect.bottom = rect.top + lastP2Height - 1;
		Vector2 texcoords[4] = 
		{
			Vector2(0.0f,0.0f),
			Vector2(0.0f,1.0f),
			Vector2(1.0f,1.0f),
			Vector2(1.0f,0.0f)
		};
		MpwFrameSub sub;
		createVBs(rect,texcoords,sub);
		sub.texture = pTextureManager->createTextureFromImage(image,
			&rect,
			FO_POINT,
			FO_POINT,
			FO_NONE, 
			TAM_CLAMP_TO_EDGE, 
			TAM_CLAMP_TO_EDGE);
		if( NULL == sub.texture)
			return false;
		list.push_back(sub);

		//____________
		//|ir.rc|     |
		//|     | 1   |
		//|-----------|
		//| 2   |  3  |
		//------------|
		Rect remainderRect[3];
		remainderRect[0].left = rect.right + 1;
		remainderRect[0].top = rect.top;
		remainderRect[0].right = rc.right;
		remainderRect[0].bottom = rect.bottom;
		remainderRect[1].left = rect.left;
		remainderRect[1].top = rect.bottom + 1;
		remainderRect[1].right = rect.right;
		remainderRect[1].bottom = rc.bottom;
		remainderRect[2].left = rect.right + 1;
		remainderRect[2].top = rect.bottom + 1;
		remainderRect[2].right = rc.right;
		remainderRect[2].bottom = rc.bottom;
		for(uint i = 0;i < 3;i++)
		{
			if(remainderRect[i].right >= remainderRect[i].left && remainderRect[i].bottom >= remainderRect[i].top)
			{
				bool ret = splitImageTo2N(pTextureManager,image,list,remainderRect[i],imageWidth,imageHeight,level + 1,bSplit);
				if( !ret) return false;
			}
		}

		return true;
	}


	bool	MpwFrame::initialize(IRenderSystem *pRenderSystem,xs::Stream* pStream,const std::string& strType,bool bSplit)
	{
		Image image;
		if(image.load(pStream,strType))
		{
			return initialize(pRenderSystem,image,bSplit);
		}

		return false;
	}

	bool	MpwFrame::initialize(IRenderSystem *pRenderSystem,Image& image,bool bSplit)
	{
		m_pRenderSystem = pRenderSystem;
		ITextureManager *pTextureManager = pRenderSystem->getTextureManager();

		uint imageWidth = image.getWidth();
		uint imageHeight = image.getHeight();

		/*
		unpack alpha values
		*/
		m_transparentBytesPerLine = ((imageWidth + 7) & ~7) >> 3;
		uint alphaSize = m_transparentBytesPerLine * imageHeight;
		m_pTransparentData = new uchar[alphaSize];
		memset(m_pTransparentData,0,alphaSize);
		if(image.hasFlag(IF_COMPRESSED))
		{
			PixelFormat pf = image.getFormat();
			switch(pf)
			{
			case PF_RGB_DXT1:
				break;
			case PF_DXT1:
				{
					uint w = (imageWidth >> 2);
					uint h = (imageHeight >> 2);
					MemoryStream stream(image.getData(),(w * h) << 3);
					for(uint m = 0;m < h;m++)
						for(uint n = 0;n < w;n++)
						{
							uchar c[8];
							stream.read(c,8);
							ushort c0,c1;
							uint bits;
							c0 = c[0] + (c[1] << 8);
							c1 = c[2] + (c[3] << 8);
							bits = (uint)c[4] + (((uint)c[5] + (((uint)c[6] + ((uint)c[7] << 8)) << 8)) << 8);
							for(uint i = 0;i < 4;i++)
								for(uint j = 0;j < 4;j++)
								{
									uchar code = ((bits >> (((j << 2) + i) << 1)) & 0x3);
									if(code == 3 && c0 <= c1)
									{
										m_pTransparentData[((m << 2) + j) * m_transparentBytesPerLine + (((n << 2) + i) >> 3)] |= (1 << (((n << 2) + i) % 8));
									}
								}
						}
				}
				break;
			case PF_DXT3:
				{
					uint w = (imageWidth >> 2);
					uint h = (imageHeight >> 2);
					MemoryStream stream(image.getData(),(w * h) << 4);
					for(uint m = 0;m < h;m++)
						for(uint n = 0;n < w;n++)
						{
							uchar a[8];
							uchar c[8];
							stream.read(a,8);
							stream.read(c,8);
							unsigned __int64 bits = (((((((((((((__int64)(a[7] << 8) + (__int64)a[6]) << 8) + (__int64)a[5]) << 8) + (__int64)a[4]) << 8) + (__int64)a[3]) << 8) + (__int64)a[2]) << 8) + (__int64)a[1]) << 8) + (__int64)a[0];
							for(uint i = 0;i < 4;i++)
								for(uint j = 0;j < 4;j++)
								{
									uchar code = ((bits >> (((j << 2) + i) << 2)) & 0xF);
									if(code == 0)
									{
										m_pTransparentData[((m << 2) + j) * m_transparentBytesPerLine + (((n << 2) + i) >> 3)] |= (1 << (((n << 2) + i) % 8));
									}
								}
						}
				}
				break;
			}
		}
		else
		{
			for(uint i = 0;i < imageHeight;i++)
				for(uint j = 0;j < imageWidth;j++)
				{
					ColorValue c = image.getColorAt(j,i,0);
					if(Math::RealEqual(c.a,0))
					{
						m_pTransparentData[i * m_transparentBytesPerLine + (j >> 3)] |= 1 << (j % 8);
					}
				}
		}

		m_width = imageWidth;
		m_height = imageHeight;

		if(m_pRenderSystem->getCapabilities()->hasCapability(RSC_NON_POWER_OF_2_TEXTURES))
		{
			m_next_power_of_two_width = m_width;
			m_next_power_of_two_height = m_height;
		}
		else
		{
			m_next_power_of_two_width = Math::NextP2(imageWidth);
			m_next_power_of_two_height = Math::NextP2(imageHeight);
		}

		if( ! splitImageTo2N(pTextureManager,image,m_vMpwFrameSub,Rect(0,0,imageWidth - 1,imageHeight - 1),imageWidth,imageHeight,0,bSplit) )
			return false;

		m_memorySize = 0;
		MpwFrameSubList::iterator begin = m_vMpwFrameSub.begin();
		MpwFrameSubList::iterator end = m_vMpwFrameSub.end();
		MpwFrameSubList::iterator it = begin;
		while(it != end)
		{
			const MpwFrameSub&	sub = (*it);
			m_memorySize += sub.texture->getMemorySize();
			++it;
		}

		return true;
	}

	bool	MpwFrame::initialize(IRenderSystem *pRenderSystem,const std::string& strImageFilename,bool bSplit)
	{
		Image image;
		if(image.load(strImageFilename))
		{
			return initialize(pRenderSystem,image,bSplit);
		}

		return false;
	}

	void 	MpwFrame::finalize()
	{
		safeDeleteArray(m_pTransparentData);

		MpwFrameSubList::iterator begin = m_vMpwFrameSub.begin();
		MpwFrameSubList::iterator end = m_vMpwFrameSub.end();
		MpwFrameSubList::iterator it = begin;
		while(it != end)
		{
			MpwFrameSub& sub = (*it);
			safeRelease(sub.texture);
			safeRelease(sub.vb);
			safeRelease(sub.vbTexcoord);
			++it;
		}
		m_vMpwFrameSub.clear();
	}

	void 	MpwFrame::render(float alpha)
	{
		PP_BY_NAME("MpwFrame::render");
		if(!m_pRenderSystem)return;
		if(m_vMpwFrameSub.empty())return;

		ColorValue c = m_pRenderSystem->getColor();
		m_pRenderSystem->setColor(ColorValue(1,1,1,alpha));
		MpwFrameSubList::iterator begin = m_vMpwFrameSub.begin();
		MpwFrameSubList::iterator end = m_vMpwFrameSub.end();
		MpwFrameSubList::iterator it = begin;
		while(it != end)
		{
			const MpwFrameSub&	sub = (*it);
			m_pRenderSystem->setTexture(0,sub.texture);
			//m_pRenderSystem->beginPrimitive(PT_QUADS);
			//m_pRenderSystem->setTexcoord(sub.t[0]);m_pRenderSystem->sendVertex(sub.v[0]);
			//m_pRenderSystem->setTexcoord(sub.t[1]);m_pRenderSystem->sendVertex(sub.v[1]);
			//m_pRenderSystem->setTexcoord(sub.t[2]);m_pRenderSystem->sendVertex(sub.v[2]);
			//m_pRenderSystem->setTexcoord(sub.t[3]);m_pRenderSystem->sendVertex(sub.v[3]);
			//m_pRenderSystem->endPrimitive();
			//m_pRenderSystem->setTexture(0,0);

			m_pRenderSystem->setTexcoordVertexBuffer(0,sub.vbTexcoord);
			m_pRenderSystem->setVertexVertexBuffer(sub.vb);
			m_pRenderSystem->setTexture(0,sub.texture);
			m_pRenderSystem->drawPrimitive( PT_TRIANGLE_FAN, 0, 4);
			++it;
		}
		m_pRenderSystem->setColor(c);
		m_pRenderSystem->setVertexVertexBuffer(0);
		m_pRenderSystem->setTexcoordVertexBuffer(0,0);
		m_pRenderSystem->setTexture(0,0);
	}

	bool	MpwFrame::isTransparent(int x,int y)
	{
		return !m_pTransparentData ? false : (m_pTransparentData[y * m_transparentBytesPerLine + (x >> 3)] & (1 << (x % 8))) != 0;
	}

	size_t MpwFrame::getMemorySize()
	{
		return m_memorySize;
	}

	MpwFrame::MpwFrame() :
		m_next_power_of_two_width(0)
		,m_next_power_of_two_height(0)
		,m_width(0)
		,m_height(0)
		,m_pTransparentData(0),m_pRenderSystem(0),m_memorySize(0)
	{
	}

	MpwFrame::~MpwFrame()
	{
	}
}