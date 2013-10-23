#include "StdAfx.h"
#include "Image.h"
#include "ImageRect.h"

namespace xs
{
	#define IMAGE_RECT_SPLIT_DEPTH	2

	void 	ImageRect::splitImageTo2N(ITextureManager *pTextureManager,const Image& image,ImageRectList& list,const Rect& rc,uint imageWidth,uint imageHeight,uint level,bool bSplit)
	{
		uint nextP2Width = Math::NextP2(rc.right - rc.left + 1);
		uint nextP2Height = Math::NextP2(rc.bottom - rc.top + 1);

		if(m_pRenderSystem->getCapabilities()->hasCapability(RSC_NON_POWER_OF_2_TEXTURES) || (imageWidth == nextP2Width && imageHeight == nextP2Height))
		{
			ImageRect2N ir;
			ir.rc = rc;
			ir.texcoord[0].x = 0;ir.texcoord[0].y = 0;
			ir.texcoord[1].x = 0;ir.texcoord[1].y = 1;
			ir.texcoord[2].x = 1;ir.texcoord[2].y = 1;
			ir.texcoord[3].x = 1;ir.texcoord[3].y = 0;
			ir.texture = pTextureManager->createTextureFromImage(image,0,FO_POINT,FO_POINT);
			if(ir.texture)list.push_back(ir);
			return;
		}

		//对于IMAGE_RECT_SPLIT_DEPTH == 2的情况，就是限制在分割成的图片最多不超过3 * 3
		if(level >= IMAGE_RECT_SPLIT_DEPTH || !bSplit)
		{
			float xRatio = (rc.right - rc.left + 1) / (float)nextP2Width;
			float yRatio = (rc.bottom - rc.top + 1) / (float)nextP2Height;
			ImageRect2N ir;
			ir.rc = rc;
			ir.texcoord[0].x = 0;ir.texcoord[0].y = 0;
			ir.texcoord[1].x = 0;ir.texcoord[1].y = yRatio;
			ir.texcoord[2].x = xRatio;ir.texcoord[2].y = yRatio;
			ir.texcoord[3].x = xRatio;ir.texcoord[3].y = 0;
			ir.texture = pTextureManager->createTextureFromImage(image,&ir.rc,nextP2Width,nextP2Height,FO_POINT,FO_POINT);
			if(ir.texture)list.push_back(ir);
			return;
		}

		uint lastP2Width = Math::LastP2(rc.right - rc.left + 1);
		uint lastP2Height = Math::LastP2(rc.bottom - rc.top + 1);
		ImageRect2N ir;
		ir.rc.left = rc.left;
		ir.rc.top = rc.top;
		ir.rc.right = ir.rc.left + lastP2Width - 1;
		ir.rc.bottom = ir.rc.top + lastP2Height - 1;
		ir.texcoord[0].x = 0;ir.texcoord[0].y = 0;
		ir.texcoord[1].x = 0;ir.texcoord[1].y = 1;
		ir.texcoord[2].x = 1;ir.texcoord[2].y = 1;
		ir.texcoord[3].x = 1;ir.texcoord[3].y = 0;
		ir.texture = pTextureManager->createTextureFromImage(image,&ir.rc,FO_POINT,FO_POINT);
		if(ir.texture)list.push_back(ir);
		//____________
		//|ir.rc|     |
		//|     | 1   |
		//|-----------|
		//| 2   |  3  |
		//------------|
		Rect remainderRect[3];
		remainderRect[0].left = ir.rc.right + 1;
		remainderRect[0].top = ir.rc.top;
		remainderRect[0].right = rc.right;
		remainderRect[0].bottom = ir.rc.bottom;
		remainderRect[1].left = rc.left;
		remainderRect[1].top = ir.rc.bottom + 1;
		remainderRect[1].right = ir.rc.right;
		remainderRect[1].bottom = rc.bottom;
		remainderRect[2].left = ir.rc.right + 1;
		remainderRect[2].top = ir.rc.bottom + 1;
		remainderRect[2].right = rc.right;
		remainderRect[2].bottom = rc.bottom;
		for(uint i = 0;i < 3;i++)
		{
			if(remainderRect[i].right >= remainderRect[i].left && remainderRect[i].bottom >= remainderRect[i].top)
			{
				splitImageTo2N(pTextureManager,image,list,remainderRect[i],imageWidth,imageHeight,level + 1,bSplit);
			}
		}
	}


	bool	ImageRect::initialize(IRenderSystem *pRenderSystem,xs::Stream* pStream,const std::string& strType,bool bSplit,const ColorValue* pColorKey)
	{
		Image image;
		if(image.load(pStream,strType))
		{
			return initialize(pRenderSystem,image,bSplit,pColorKey);
		}

		return false;
	}

	bool	ImageRect::initialize(IRenderSystem *pRenderSystem,Image& image,bool bSplit,const ColorValue *pColorKey)
	{
		m_pRenderSystem = pRenderSystem;
		ITextureManager *pTextureManager = pRenderSystem->getTextureManager();

		if(pColorKey)
		{
			image.setColorKey(*pColorKey);
			m_blendMode = IR_TRANSPARENT;
		}
		else
		{
			if(image.getHasAlpha())
			{
				m_blendMode = IR_ALPHA_BLEND;
			}
		}

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

		//
		/*FILE *fp = fopen("1.raw","wb+");
		if(fp)
		{
			uchar a0 = 0,a1 = 255;
			for(uint i = 0;i < imageHeight;i++)
			for(uint j = 0;j < imageWidth;j++)
			{
				if(isTransparent(j,i))
				{
					fwrite(&a1,1,1,fp);
				}
				else
				{
					fwrite(&a0,1,1,fp);
				}
			}
			fclose(fp);
		}*/

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

		splitImageTo2N(pTextureManager,image,m_vImageRect,Rect(0,0,imageWidth - 1,imageHeight - 1),imageWidth,imageHeight,0,bSplit);

		uint need,waste,total;
		need = imageWidth * imageHeight;
		total = 0;

		ImageRectList::iterator begin = m_vImageRect.begin();
		ImageRectList::iterator end = m_vImageRect.end();
		ImageRectList::iterator it = begin;
		while(it != end)
		{
			const ImageRect2N& ir = (*it);
			total += ir.texture->width() * ir.texture->height();
			++it;
		}

		waste = total - need;

		//TRACE0_L0("\nNeed = %d,Total = %d,Waste = %d,Ratio = %.2f,TextureNum = %d\n",need,total,waste,waste / (float)total,m_vImageRect.size());

		return true;
	}

	bool	ImageRect::initialize(IRenderSystem *pRenderSystem,const std::string& strImageFilename,bool bSplit,const ColorValue* pColorKey)
	{
		Image image;
		if(image.load(strImageFilename))
		{
			return initialize(pRenderSystem,image,bSplit,pColorKey);
		}
		
		return false;
	}

	void 	ImageRect::finalize()
	{
		safeDeleteArray(m_pTransparentData);

		ImageRectList::iterator begin = m_vImageRect.begin();
		ImageRectList::iterator end = m_vImageRect.end();
		ImageRectList::iterator it = begin;
		while(it != end)
		{
			safeRelease((*it).texture);
			++it;
		}
		m_vImageRect.clear();
	}

	void 	ImageRect::render(const Rect *pSourceRect)
	{
		if(!m_pRenderSystem)return;

		uint size = m_vImageRect.size();
		if(size > 1 && pSourceRect)
		{
			Error("ImageRect::render split image can't use partial Blt\n");
			return;
		}

		bool bAlphaCheck;
		CompareFunction cf;
		float value;
		SceneBlendFactor src,dst;
		bool bDepthBufferWriteEnabled;

		switch(m_blendMode)
		{
		case IR_OPAQUE:
			break;
		case IR_TRANSPARENT:
			bAlphaCheck = m_pRenderSystem->alphaCheckEnabled();
			m_pRenderSystem->getAlphaFunction(cf,value);
			m_pRenderSystem->setAlphaCheckEnabled(true);
			m_pRenderSystem->setAlphaFunction(CMPF_GREATER,0.1f);
			break;
		case IR_ALPHA_BLEND:
			m_pRenderSystem->getSceneBlending(src,dst);
			m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
			bDepthBufferWriteEnabled = m_pRenderSystem->isDepthBufferWriteEnabled();
			m_pRenderSystem->setDepthBufferWriteEnabled(false);
			break;
		}

		if(size == 1)
		{
			ImageRectList::iterator begin = m_vImageRect.begin();
			const ImageRect2N&	ir = (*begin);
			Rect rcDest = ir.rc;
			Vector2 texcoord [4];

			if(!pSourceRect)
			{
				texcoord[0] = ir.texcoord[0];
				texcoord[1] = ir.texcoord[1];
				texcoord[2] = ir.texcoord[2];
				texcoord[3] = ir.texcoord[3];
			}

			if(pSourceRect)
			{
				if(ir.rc == *pSourceRect)
				{
					texcoord[0] = ir.texcoord[0];
					texcoord[1] = ir.texcoord[1];
					texcoord[2] = ir.texcoord[2];
					texcoord[3] = ir.texcoord[3];
				}
				else
				{
					if(pSourceRect->left > rcDest.left)rcDest.left = pSourceRect->left;
					if(pSourceRect->top > rcDest.top)rcDest.top = pSourceRect->top;
					if(pSourceRect->right < rcDest.right)rcDest.right = pSourceRect->right;
					if(pSourceRect->bottom < rcDest.bottom)rcDest.bottom = pSourceRect->bottom;

					float leftTexcoord = rcDest.left / (float)m_next_power_of_two_width;
					float topTexcoord = rcDest.top / (float)m_next_power_of_two_height;
					float rightTexcoord = (rcDest.right + 1) / (float)m_next_power_of_two_width;
					float bottomTexcoord = (rcDest.bottom + 1) / (float)m_next_power_of_two_height;

					texcoord[0].x = leftTexcoord;texcoord[0].y = topTexcoord;
					texcoord[1].x = leftTexcoord;texcoord[1].y = bottomTexcoord;
					texcoord[2].x = rightTexcoord;texcoord[2].y = bottomTexcoord;
					texcoord[3].x = rightTexcoord;texcoord[3].y = topTexcoord;

					rcDest.moveTo00();
				}
			}

			m_pRenderSystem->setTexture(0,ir.texture);
			m_pRenderSystem->beginPrimitive(PT_QUADS);

			//by yhc 加一导致象素会模糊,修正以后变得清晰
			/*m_pRenderSystem->setTexcoord(texcoord[0]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.left,rcDest.top));
			m_pRenderSystem->setTexcoord(texcoord[1]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.left,rcDest.bottom + 1));
			m_pRenderSystem->setTexcoord(texcoord[2]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.right + 1,rcDest.bottom + 1));
			m_pRenderSystem->setTexcoord(texcoord[3]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.right + 1,rcDest.top));*/
			m_pRenderSystem->setTexcoord(texcoord[0]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.left,rcDest.top));
			m_pRenderSystem->setTexcoord(texcoord[1]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.left,rcDest.bottom));
			m_pRenderSystem->setTexcoord(texcoord[2]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.right,rcDest.bottom ));
			m_pRenderSystem->setTexcoord(texcoord[3]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.right,rcDest.top));
			m_pRenderSystem->endPrimitive();
			m_pRenderSystem->setTexture(0,0);
		}
		else
		{
			ImageRectList::iterator begin = m_vImageRect.begin();
			ImageRectList::iterator end = m_vImageRect.end();
			ImageRectList::iterator it = begin;
			while(it != end)
			{
				const ImageRect2N&	ir = (*it);
				m_pRenderSystem->setTexture(0,ir.texture);
				m_pRenderSystem->beginPrimitive(PT_QUADS);
				//by yhc 加一导致象素会模糊,修正以后变得清晰
				/*m_pRenderSystem->setTexcoord(ir.texcoord[0]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.left,ir.rc.top));
				m_pRenderSystem->setTexcoord(ir.texcoord[1]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.left,ir.rc.bottom + 1));
				m_pRenderSystem->setTexcoord(ir.texcoord[2]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.right + 1,ir.rc.bottom + 1));
				m_pRenderSystem->setTexcoord(ir.texcoord[3]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.right + 1,ir.rc.top));*/
				m_pRenderSystem->setTexcoord(ir.texcoord[0]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.left,ir.rc.top));
				m_pRenderSystem->setTexcoord(ir.texcoord[1]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.left,ir.rc.bottom));
				m_pRenderSystem->setTexcoord(ir.texcoord[2]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.right,ir.rc.bottom));
				m_pRenderSystem->setTexcoord(ir.texcoord[3]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.right,ir.rc.top));

				m_pRenderSystem->endPrimitive();
				++it;
			}
			m_pRenderSystem->setTexture(0,0);
		}

		switch(m_blendMode)
		{
		case IR_OPAQUE:
			break;
		case IR_TRANSPARENT:
			m_pRenderSystem->setAlphaCheckEnabled(bAlphaCheck);
			m_pRenderSystem->setAlphaFunction(cf,value);
			break;
		case IR_ALPHA_BLEND:
			m_pRenderSystem->setDepthBufferWriteEnabled(bDepthBufferWriteEnabled);
			m_pRenderSystem->setSceneBlending(src,dst);
			break;
		}
	}

	void 	ImageRect::render(int x,int y,float z,const Rect *pSourceRect)
	{
		if(!m_pRenderSystem)return;

		Matrix4 mtxWorldSave = m_pRenderSystem->getWorldMatrix();
		Matrix4 mtxWorld;
		mtxWorld.makeTrans(x,y,z);

		uint size = m_vImageRect.size();
		if(size > 1 && pSourceRect)
		{
			Error("ImageRect::render split image can't use partial Blt\n");
			return;
		}

		m_pRenderSystem->setWorldMatrix(mtxWorldSave * mtxWorld);

		bool bAlphaCheck;
		CompareFunction cf;
		float value;
		SceneBlendFactor src,dst;
		bool bDepthBufferWriteEnabled;

		switch(m_blendMode)
		{
		case IR_OPAQUE:
			break;
		case IR_TRANSPARENT:
			bAlphaCheck = m_pRenderSystem->alphaCheckEnabled();
			m_pRenderSystem->getAlphaFunction(cf,value);
			m_pRenderSystem->setAlphaCheckEnabled(true);
			m_pRenderSystem->setAlphaFunction(CMPF_GREATER,0.1f);
			break;
		case IR_ALPHA_BLEND:
			m_pRenderSystem->getSceneBlending(src,dst);
			m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
			bDepthBufferWriteEnabled = m_pRenderSystem->isDepthBufferWriteEnabled();
			m_pRenderSystem->setDepthBufferWriteEnabled(false);
			break;
		}

		if(size == 1)
		{
			ImageRectList::iterator begin = m_vImageRect.begin();
			const ImageRect2N&	ir = (*begin);
			Rect rcDest = ir.rc;
			Vector2 texcoord [4];

			if(!pSourceRect)
			{
				texcoord[0] = ir.texcoord[0];
				texcoord[1] = ir.texcoord[1];
				texcoord[2] = ir.texcoord[2];
				texcoord[3] = ir.texcoord[3];
			}

			if(pSourceRect)
			{
				if(ir.rc == *pSourceRect)
				{
					texcoord[0] = ir.texcoord[0];
					texcoord[1] = ir.texcoord[1];
					texcoord[2] = ir.texcoord[2];
					texcoord[3] = ir.texcoord[3];
				}
				else
				{
					if(pSourceRect->left > rcDest.left)rcDest.left = pSourceRect->left;
					if(pSourceRect->top > rcDest.top)rcDest.top = pSourceRect->top;
					if(pSourceRect->right < rcDest.right)rcDest.right = pSourceRect->right;
					if(pSourceRect->bottom < rcDest.bottom)rcDest.bottom = pSourceRect->bottom;

					float leftTexcoord = rcDest.left / (float)m_next_power_of_two_width;
					float topTexcoord = rcDest.top / (float)m_next_power_of_two_height;
					float rightTexcoord = (rcDest.right + 1) / (float)m_next_power_of_two_width;
					float bottomTexcoord = (rcDest.bottom + 1) / (float)m_next_power_of_two_height;

					texcoord[0].x = leftTexcoord;texcoord[0].y = topTexcoord;
					texcoord[1].x = leftTexcoord;texcoord[1].y = bottomTexcoord;
					texcoord[2].x = rightTexcoord;texcoord[2].y = bottomTexcoord;
					texcoord[3].x = rightTexcoord;texcoord[3].y = topTexcoord;

					rcDest.moveTo00();
				}
			}

			m_pRenderSystem->setTexture(0,ir.texture);
			m_pRenderSystem->beginPrimitive(PT_QUADS);
			m_pRenderSystem->setTexcoord(texcoord[0]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.left,rcDest.top));
			m_pRenderSystem->setTexcoord(texcoord[1]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.left,rcDest.bottom + 1));
			m_pRenderSystem->setTexcoord(texcoord[2]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.right + 1,rcDest.bottom + 1));
			m_pRenderSystem->setTexcoord(texcoord[3]);
			m_pRenderSystem->sendVertex(Vector2(rcDest.right + 1,rcDest.top));
			m_pRenderSystem->endPrimitive();
			m_pRenderSystem->setTexture(0,0);
		}
		else
		{
			ImageRectList::iterator begin = m_vImageRect.begin();
			ImageRectList::iterator end = m_vImageRect.end();
			ImageRectList::iterator it = begin;
			while(it != end)
			{
				const ImageRect2N&	ir = (*it);
				m_pRenderSystem->setTexture(0,ir.texture);
				m_pRenderSystem->beginPrimitive(PT_QUADS);
				m_pRenderSystem->setTexcoord(ir.texcoord[0]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.left,ir.rc.top));
				m_pRenderSystem->setTexcoord(ir.texcoord[1]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.left,ir.rc.bottom + 1));
				m_pRenderSystem->setTexcoord(ir.texcoord[2]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.right + 1,ir.rc.bottom + 1));
				m_pRenderSystem->setTexcoord(ir.texcoord[3]);
				m_pRenderSystem->sendVertex(Vector2(ir.rc.right + 1,ir.rc.top));
				m_pRenderSystem->endPrimitive();
				++it;
			}
			m_pRenderSystem->setTexture(0,0);
		}

		switch(m_blendMode)
		{
		case IR_OPAQUE:
			break;
		case IR_TRANSPARENT:
			m_pRenderSystem->setAlphaCheckEnabled(bAlphaCheck);
			m_pRenderSystem->setAlphaFunction(cf,value);
			break;
		case IR_ALPHA_BLEND:
			m_pRenderSystem->setDepthBufferWriteEnabled(bDepthBufferWriteEnabled);
			m_pRenderSystem->setSceneBlending(src,dst);
			break;
		}

		m_pRenderSystem->setWorldMatrix(mtxWorldSave);
	}

	bool	ImageRect::isTransparent(int x,int y)
	{
		return !m_pTransparentData ? false : (m_pTransparentData[y * m_transparentBytesPerLine + (x >> 3)] & (1 << (x % 8))) != 0;
	}
	
	ImageRect::ImageRect() : m_blendMode(IR_OPAQUE)
		,m_next_power_of_two_width(0)
		,m_next_power_of_two_height(0)
		,m_width(0)
		,m_height(0)
		,m_pTransparentData(0),m_pRenderSystem(0)
	{
	}

	ImageRect::~ImageRect()
	{
		finalize();//防止忘记调用finalize()函数
	}
}