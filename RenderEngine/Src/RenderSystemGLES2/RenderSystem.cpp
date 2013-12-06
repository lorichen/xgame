//todo: VBO vs.NonVBO - BufferManager

#include "StdAfx.h"
#include "RenderSystem.h"
#include "Adapter.h"
#include "BufferManager.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
//#include "DefaultBufferManager.h"
#include "TextureManager.h"
#include "Texture.h"
//#include "ShaderManager.h"
#include "HighLevelShaderManager.h"
#include "ShaderProgramManagerOGL.h"
#include "RenderTarget.h"
#include "pbuffer.h"
//#include "AVIVideoObject.h"
#include "HardwareCursorManagerOGL.h"

#define DEFAULT_TEX_SIZE 64

namespace xs
{
	bool	RenderSystem::m_sCapabilitiesInit = false;
	std::list<IRenderSystem*>	RenderSystem::m_vRenderSystems;
	extern GLint getAddressingMode(TextureAddressingMode tam);
	extern TextureAddressingMode getAddressingMode(GLint);
	
	extern const AttrInfo* getAttrInfo(VetextAttr attr);
	extern bool   getAttrGLInfo(VertexElementType type,GLenum&  glType,GLint& components,GLboolean& normalized);
	extern GLuint getAttrLocation(unsigned int usage,unsigned int index);

	// Convenience macro from ARB_vertex_buffer_object spec
	#define VBO_BUFFER_OFFSET(i) ((uchar *)NULL + (i))

	static GLenum getPrimitiveType(PrimitiveType pt) 
	{
		switch (pt) 
		{
		case PT_LINES:
			return GL_LINES;

		case PT_LINE_STRIP:
			return GL_LINE_STRIP;

		case PT_LINE_LOOP:
			return GL_LINE_LOOP;

		case PT_TRIANGLES:
			return GL_TRIANGLES;

		case PT_TRIANGLE_STRIP:
			return GL_TRIANGLE_STRIP;

		case PT_TRIANGLE_FAN:
			return GL_TRIANGLE_FAN;

		//case PT_QUADS:
			//return GL_QUADS;

		//case PT_QUAD_STRIP:
		//	return GL_QUAD_STRIP;

		case PT_POINTS:
			return GL_POINTS;

		default:
			assert(0);
			return GL_POINTS;
		}

		//should never get here.
		return PT_TRIANGLES;
	}

	void RenderSystem::release()
	{
		close();
		
		//类静态变量是魔鬼!!
		m_sCapabilitiesInit = false;

		std::list<IRenderSystem*>::const_iterator citer = m_vRenderSystems.begin();
		for (; citer != m_vRenderSystems.end(); citer++)
		{
			(*citer)->release();
		}
		m_vRenderSystems.clear();
	
        delete this;
    }

	RenderSystem::RenderSystem()
	{
		m_pIndexBuffer = 0;

		m_currentRenderTarget = 0;
		m_pCurrentRenderTarget = 0;
		m_stencilBuffer = 0;
		m_lineWidth = 1.0f;
		m_scissorEnabled = false;
        
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		m_hDC = 0;
		m_hWnd = 0;
		m_hRC = 0;
#else
        m_hWnd = 0;
        m_pContext = 0;
#endif

		m_surfaceDiffuse = ColorValue(0.8f,0.8f,0.8f,1.0f);
		m_textureUnit = 0;
		m_pOverlayRenderTarget = 0;

		m_pWhiteTex = 0;
		//m_pCurrentShaderProgram = 0;

		for(int i = 0;i < ESP_NUM;++i)
		{
			m_pInnerShader[i] = 0;
		}
	}

	void	RenderSystem::close()
	{
		_releaseInnerShader();
		//--other uninit----------------
		if(m_pWhiteTex)
		{
			TextureManager::Instance()->releaseTexture(m_pWhiteTex);
			m_pWhiteTex = 0;
		}
		//-------------------------

		RenderTargetType::iterator begin = m_vRenderTarget.begin();
		RenderTargetType::iterator end = m_vRenderTarget.end();
		for(RenderTargetType::iterator it = begin;it != end;++it)
		{
			if(it->first != m_currentRenderTarget)
			{
				uint currentRenderTarget = m_currentRenderTarget;
				setCurrentRenderTarget(it->first);
				//wglDeleteContext(it->second->rc);
				//ReleaseDC((HWND)it->first,it->second->dc);
				setCurrentRenderTarget(currentRenderTarget);
				it->second->release();
			}
		}
		RenderTargetType::iterator it = m_vRenderTarget.find(m_currentRenderTarget);
		if(it != m_vRenderTarget.end())
		{
			//wglDeleteContext(it->second->rc);
			//ReleaseDC((HWND)it->first,it->second->dc);
			it->second->release();
		}
		m_vRenderTarget.clear();

		m_vRenderSystems.remove(this);

		//这里的释放值得商榷
		if(m_vRenderSystems.empty())
		{
			//ShaderProgramManagerOGL::Instance()->getLowLevelShaderManager()->releaseAllShader();
			ShaderProgramManagerOGL::Instance()->getHighLevelShaderManager()->releaseAllShader();
			
			ITextureManager *pTextureManager = getTextureManager();
			pTextureManager->releaseAll();
		}

#if (TARGET_PLATFORM == PLATFORM_WIN32)
		//wglMakeCurrent(0,0);
		eglDestroySurface(m_eglDisplay, m_eglSurface);
		eglTerminate(m_eglDisplay);
		if (m_hDC) ReleaseDC(m_hWnd, m_hDC);
#else
        
#endif
	}

	void	RenderSystem::setDefaultMatrix(uint cx,uint cy)
	{
		if(cx == 0 || cy == 0)return;

        setViewport(0, 0, cx, cy);
		//glViewport(0,0,cx,cy);

		GLfloat fAspectRatio = (GLfloat)cx / (GLfloat)cy;
		
		/*
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0f,fAspectRatio,1.0f,1000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0,0,1,0,0,0,0,1,0);
		*/
		if(!m_pCurrentRenderTarget->m_b2D)
		{
			Matrix4 mtxW = Matrix4::IDENTITY;
			mtxW._m[0]=1.0000000;	
			mtxW._m[1]=-0.00000000;	
			mtxW._m[2]=0.00000000;	
			mtxW._m[3]=0.00000000;	
			mtxW._m[4]=0.00000000;	
			mtxW._m[5]=1.0000000;	
			mtxW._m[6]=0.00000000;	
			mtxW._m[7]=-0.00000000;	
			mtxW._m[8]=-0.00000000;	
			mtxW._m[9]=0.00000000;	
			mtxW._m[10]=1.0000000;	
			mtxW._m[11]=-1.0000000;	
			mtxW._m[12]=0.00000000;	
			mtxW._m[13]=0.00000000;	
			mtxW._m[14]=0.00000000;	
			mtxW._m[15]=1.0000000;	
			setViewMatrix(mtxW);

			Degree d(60.0f);
			Radian r(d);
			setProjectionMatrix(Matrix4::perspectiveProjection(r,fAspectRatio,1.0f,1000.0f));
		}
		
		//glClearColor(0.223f,0.427f,0.647f,1);
		glClearDepthf(1.0f);
	}

	bool RenderSystem::checkMinGLVersion(const std::string& v) const
	{
		uint first, second, third;
		uint cardFirst, cardSecond, cardThird;
		if(v == m_strVersion)
			return true;

		std::string::size_type pos = v.find(".");
		if(pos == std::string::npos)
			return false;

		std::string::size_type pos1 = v.rfind(".");
		if(pos1 == std::string::npos)
			return false;

		first = ::atoi(v.substr(0, pos).c_str());
		second = ::atoi(v.substr(pos + 1, pos1 - (pos + 1)).c_str());
		third = ::atoi(v.substr(pos1 + 1, v.length()).c_str());

		pos = m_strVersion.find(".");
		if(pos == std::string::npos)
			return false;

		pos1 = m_strVersion.rfind(".");
		if(pos1 == std::string::npos)
			return false;

		cardFirst  = ::atoi(m_strVersion.substr(0, pos).c_str());
		cardSecond = ::atoi(m_strVersion.substr(pos + 1, pos1 - (pos + 1)).c_str());
		cardThird  = ::atoi(m_strVersion.substr(pos1 + 1, m_strVersion.length()).c_str());

		if(first <= cardFirst && second <= cardSecond && third <= cardThird)
			return true;

		return false;
	}

	void	RenderSystem::initCapabilities()
	{
		if(m_sCapabilitiesInit)return;
		m_sCapabilitiesInit = true;

		Trace("=========OpenGL Information==========\n");
		std::string str;
		str = (char*)glGetString(GL_VERSION);
		m_strVersion = str.substr(0,str.find(" "));
		str = (char*)glGetString(GL_VENDOR);
		m_strVendor = str.substr(0,str.find(" "));
		m_strRenderer = (char*)glGetString(GL_RENDERER);
		m_strExtensions = (char*)glGetString(GL_EXTENSIONS);
		Trace("Vendor: "<<m_strVendor.c_str()<<endl);
		Trace("Renderer: "<<m_strRenderer.c_str()<<endl);
		std::string strExt = m_strExtensions;
		Trace("Extensions:\n");
		while(!strExt.empty())
		{
			std::string::size_type pos = strExt.find_first_of(' ');
			if(pos != std::string::npos)
			{
				std::string str = strExt.substr(0,pos);
				str += "\n";
				Trace(str.c_str());
				strExt = strExt.substr(pos + 1,strExt.size() - 1);
			}
			else
			{
				strExt += "\n";
				Trace(strExt.c_str());
				break;
			}
		}
		Trace("\n");


		// Check for blending support
		m_rsc.setCapability(RSC_BLENDING);
		Trace("OK! Blending\n");
		

		// Check for Multitexturing support and set number of texture units
		GLint units = 8;
		//glGetIntegerv( GL_MAX_TEXTURE_UNITS, &units );

		m_rsc.setNumTextureUnits(units);
		Trace("TextureUnits: "<<units<<endl);

		m_rsc.setCapability(RSC_ANISOTROPY);
		Trace("OK! Anisotropy Filter\n");
	

		// Check for DOT3 support
		m_rsc.setCapability(RSC_DOT3);
		Trace("OK! Dot3\n");
		
		// Check for cube mapping
		m_rsc.setCapability(RSC_CUBEMAPPING);
		Trace("OK! CubeMapping\n");

		// Check for hardware stencil support and set bit depth
		GLint stencil;
		glGetIntegerv(GL_STENCIL_BITS,&stencil);
		if(stencil)
		{
			m_rsc.setCapability(RSC_HWSTENCIL);
			m_rsc.setStencilBufferBitDepth(stencil);
			Trace("OK! Hardware Stencil Buffer depth = "<<stencil<<endl);
		}
		else
		{
			Trace("FAIL! Hardware Stencil Buffer\n");
		}

		// Check for VBO support
		m_rsc.setCapability(RSC_VBO);
		Trace("OK! Vertex Buffer Object\n");
		

		{
			m_rsc.setCapability(RSC_VERTEX_PROGRAM);
			Trace("OK! Vertex Program\n");

			// Vertex Program Properties
			m_rsc.setMaxVertexProgramVersion("arbvp1");
			m_rsc.setVertexProgramConstantBoolCount(0);
			m_rsc.setVertexProgramConstantIntCount(0);
			
			//m_rsc.setVertexProgramConstantFloatCount(GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB);

			//if (glewIsExtensionSupported("GL_NV_vertex_program2_option"))
			{
				m_rsc.setMaxVertexProgramVersion("vp30");
			}

			//if (glewIsExtensionSupported("GL_NV_vertex_program3"))
			{
				m_rsc.setMaxVertexProgramVersion("vp40");
			}
		}
		

		//if (glewIsExtensionSupported("GL_NV_register_combiners2") &&
		//	glewIsExtensionSupported("GL_NV_texture_shader"))
		{
			m_rsc.setCapability(RSC_FRAGMENT_PROGRAM);
			m_rsc.setMaxFragmentProgramVersion("fp20");
		}

		/*
		// check for ATI fragment shader support
		//if (glewIsExtensionSupported("GL_ATI_fragment_shader"))
		{
			m_rsc.setCapability(RSC_FRAGMENT_PROGRAM);
			m_rsc.setMaxFragmentProgramVersion("ps_1_4");
			// no boolean params allowed
			m_rsc.setFragmentProgramConstantBoolCount(0);
			// no integer params allowed
			m_rsc.setFragmentProgramConstantIntCount(0);

			// only 8 Vector4 constant floats supported
			m_rsc.setFragmentProgramConstantFloatCount(8);
		}

		if (glewIsExtensionSupported("GL_ARB_fragment_program"))
		{
			m_rsc.setCapability(RSC_FRAGMENT_PROGRAM);
			Trace("OK! Fragment Program\n");

			// Fragment Program Properties
			m_rsc.setMaxFragmentProgramVersion("arbfp1");
			m_rsc.setFragmentProgramConstantBoolCount(0);
			m_rsc.setFragmentProgramConstantIntCount(0);
			m_rsc.setFragmentProgramConstantFloatCount(
				GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB);

			if (glewIsExtensionSupported("GL_NV_fragment_program_option"))
			{
				m_rsc.setMaxFragmentProgramVersion("fp30");
			}

			if (glewIsExtensionSupported("GL_NV_fragment_program2"))
			{
				m_rsc.setMaxFragmentProgramVersion("fp40");
			}        
		}
		else
		{
			Trace("FAIL! Fragment Program\n");
		}
		

		// Check for texture compression
		if(checkMinGLVersion("1.3.0") ||
			glewIsExtensionSupported("GL_ARB_texture_compression"))
		{   
			m_rsc.setCapability(RSC_TEXTURE_COMPRESSION);
			Trace("OK! Texture Compression\n");

			// Check for dxt compression
			if(glewIsExtensionSupported("GL_EXT_texture_compression_s3tc"))
			{
				m_rsc.setCapability(RSC_TEXTURE_COMPRESSION_DXT);
			}
			// Check for vtc compression
			if(glewIsExtensionSupported("GL_NV_texture_compression_vtc"))
			{
				m_rsc.setCapability(RSC_TEXTURE_COMPRESSION_VTC);
			}
		}
		else
		{
			Trace("FAIL! Texture Compression\n");
		}
		*/

		// Scissor test is standard in GL 1.2 (is it emulated on some cards though?)
		m_rsc.setCapability(RSC_SCISSOR_TEST);
		Trace("OK! Scissor Test\n");
		// As are user clipping planes
		m_rsc.setCapability(RSC_USER_CLIP_PLANES);
		Trace("OK! User Clip Planes\n");

		// 2-sided stencil?
		//if (glewIsExtensionSupported("GL_EXT_stencil_two_side"))
		{
			m_rsc.setCapability(RSC_TWO_SIDED_STENCIL);
			Trace("OK! Two Side Stencil\n");
		}
		

		// stencil wrapping?
		//if (glewIsExtensionSupported("GL_EXT_stencil_wrap"))
		{
			m_rsc.setCapability(RSC_STENCIL_WRAP);
			Trace("OK! Stencil Wrap\n");
		}
		

		// Check for hardware occlusion support
		//if(glewIsExtensionSupported("GL_NV_occlusion_query"))
		{
			m_rsc.setCapability(RSC_HWOCCLUSION);
			Trace("OK! Hardware Occlusion Query\n");
		}
		

		// UBYTE4 always supported
		m_rsc.setCapability(RSC_VERTEX_FORMAT_UBYTE4);

		// Inifinite far plane always supported
		m_rsc.setCapability(RSC_INFINITE_FAR_PLANE);
		Trace("OK! Infinite Far Plane\n");

		// Check for non-power-of-2 texture support
		//if(glewIsExtensionSupported("GL_ARB_texture_non_power_of_two"))
		{
			m_rsc.setCapability(RSC_NON_POWER_OF_2_TEXTURES);
			Trace("OK! Non Power of 2 Textures\n");
		}
		

		// Check for float textures
		//if(glewIsExtensionSupported("GL_ATI_texture_float") ||
		//	glewIsExtensionSupported("GL_ARB_texture_float"))
		{
			m_rsc.setCapability(RSC_TEXTURE_FLOAT);
			Trace("OK! float Texture\n");
		}
		

		// 3D textures should be supported by GL 1.2, which is our minimum version
		m_rsc.setCapability(RSC_TEXTURE_3D);
		Trace("OK! 3D Texture\n");

		// Check for GLSL support
		//if(glewIsExtensionSupported("GL_ARB_fragment_shader") && glewIsExtensionSupported("GL_ARB_vertex_shader"))
		{
			m_rsc.setCapability(RSC_HIGHLEVEL_SHADER);
			Trace("OK! GLSL\n");
		}
		

		GLint maxTextureSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxTextureSize); 
		m_rsc.setMaxTextureSize(maxTextureSize);

		/*
		GLint maxVertexAttribs;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB,&maxVertexAttribs);
		m_rsc.setMaxVertexAttribs(maxVertexAttribs);
		GLint maxVertexUniformComponents;
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB,&maxVertexUniformComponents);
		m_rsc.setMaxVertexUniformComponents(maxVertexUniformComponents);
		GLint maxVaryingFloats;
		glGetIntegerv(GL_MAX_VARYING_FLOATS_ARB,&maxVaryingFloats);
		m_rsc.setMaxVaryingFloats(maxVaryingFloats);
		GLint maxCombinedTextureImageUnits;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB,&maxCombinedTextureImageUnits);
		m_rsc.setMaxCombinedTextureImageUnits(maxCombinedTextureImageUnits);
		GLint maxVertexTextureImageUnits;
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB,&maxVertexTextureImageUnits);
		m_rsc.setMaxVertexTextureImageUnits(maxVertexTextureImageUnits);
		GLint maxTextureCoords;
		glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,&maxTextureCoords);
		m_rsc.setMaxTextureCoords(maxTextureCoords);
		GLint maxFragmentUniformComponents;
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB,&maxFragmentUniformComponents);
		m_rsc.setMaxFragmentUniformComponents(maxFragmentUniformComponents);
		*/
	}

	const char* RenderSystem::getName()
	{
		static char szName[256] = "OpenGLES2";
		return szName;
	}

	RenderSystemCapabilities*	RenderSystem::getCapabilities()
	{
		return &m_rsc;
	}

	bool RenderSystem::setCurrentRenderTarget(uint hwnd)
	{
		if(m_currentRenderTarget == hwnd)return true;

		if(!hwnd)
		{
			m_currentRenderTarget = 0;
			if(m_pCurrentRenderTarget)
			{
				m_pCurrentRenderTarget->onDetach();
				m_pCurrentRenderTarget = 0;
			}
			return true;
		}

		RenderTargetType::iterator it = m_vRenderTarget.find(hwnd);
		if(it == m_vRenderTarget.end())return false;

		if(m_pCurrentRenderTarget)m_pCurrentRenderTarget->onDetach();
		//wglMakeCurrent(it->second->dc,it->second->rc);
		it->second->onAttach();
		
		m_currentRenderTarget = hwnd;
		m_pCurrentRenderTarget = it->second;

		return true;
	}

	const ITexture*	RenderSystem::getRenderTargetTexture(uint hwnd)
	{
		RenderTargetType::iterator it = m_vRenderTarget.find(hwnd);
		if(it == m_vRenderTarget.end())return 0;

		return it->second->getTexture();
	}

	bool RenderSystem::isScissorEnabled()
	{
		return m_scissorEnabled;
	}

	void RenderSystem::setScissorEnabled(bool enabled)
	{
		if(m_scissorEnabled != enabled)
		{
			m_scissorEnabled = enabled;
			if(m_scissorEnabled)
			{
				glEnable(GL_SCISSOR_TEST);
			}
			else
			{
				glDisable(GL_SCISSOR_TEST);
			}
		}
	}

	IVideoObject * RenderSystem::createVideoObject(const char * pszFilename, uint type , uint imgWidth, uint imgHeight)
	{
		assert(0);
		return 0;
	}

	uint RenderSystem::addRTT(int width,int height, bool alpha,	FilterOptions min,FilterOptions mag,
		FilterOptions mip, TextureAddressingMode s , TextureAddressingMode t)
	{
		pbuffer *ppbuffer = new pbuffer(this);
		if(ppbuffer)
		{
			bool initret = false;
			if( alpha)
				initret = ppbuffer->initialize(width,height,32,16,8,this,min, mag, mip,s,t);
			else
				initret = ppbuffer->initialize(width,height,24,16,8,this,min,mag,mip,s,t);
			if( !initret)
			{
				delete ppbuffer;
				ppbuffer = 0;
			}
		}

		if(ppbuffer)
		{
			m_vRenderTarget[ppbuffer->m_id] = ppbuffer;
			//if(m_pCurrentRenderTarget && m_pCurrentRenderTarget->rc)wglShareLists(m_pCurrentRenderTarget->rc,ppbuffer->rc);
		}

		return ppbuffer ? ppbuffer->m_id : 0;
	}

	bool RenderSystem::addRenderTarget(uint hwnd)
	{
		//if(m_vRenderTarget.find(hwnd) != m_vRenderTarget.end())return true;

		RenderTarget *pRenderTarget = initializeRenderTarget((HWND)hwnd,0);
		if(pRenderTarget)
		{
			m_vRenderTarget[(uint)hwnd] = pRenderTarget;
			return true;
		}

		return false;
	}

	bool RenderSystem::removeRenderTarget(uint hwnd)
	{
		if(m_currentRenderTarget == hwnd)return false;

		RenderTargetType::iterator it = m_vRenderTarget.find(hwnd);
		if(it == m_vRenderTarget.end())return false;

		uint currentRenderTarget = m_currentRenderTarget;
		setCurrentRenderTarget(hwnd);
		//ReleaseDC((HWND)it->first,it->second->dc);
		setCurrentRenderTarget(currentRenderTarget);
		it->second->release();

		m_vRenderTarget.erase(it);
		return true;
	}

	uint RenderSystem::MTaddRenderTarget(int layer)
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		RenderTarget *pRenderTarget = new RenderTarget(m_hDC,m_hWnd,this,m_hRC);
#else
        RenderTarget* pRenderTarget = new RenderTarget(m_hWnd,this,m_pContext);
#endif
		if(pRenderTarget)
		{
			m_vRenderTargetMT[(uint)pRenderTarget->GetFrameBufferObj()] = pRenderTarget;
			return (uint)pRenderTarget->GetFrameBufferObj();
		}
		return 0;
	}

	bool RenderSystem::MTremoveRenderTarget(uint key)
	{
		RenderTargetType::iterator it = m_vRenderTargetMT.find(key);
		if(it == m_vRenderTargetMT.end())return false;

		//wglDeleteContext(it->second->rc);
		it->second->release();

		m_vRenderTargetMT.erase(it);
		return true;
	}

	bool RenderSystem::addOverlayRenderTarget()
	{
		//m_pOverlayRenderTarget = initializeRenderTarget(m_hWnd,1);
		//return m_pOverlayRenderTarget != 0;
		
		assert(0);
		return false;
	}

	void RenderSystem::removeOverlayRenderTarget()
	{
		if(!m_pOverlayRenderTarget)return;

		if(m_pCurrentRenderTarget != m_pOverlayRenderTarget)
		{
			setOverlayRenderTarget();
			//wglDeleteContext(m_pOverlayRenderTarget->rc);
			//ReleaseDC(m_hWnd,m_pOverlayRenderTarget->dc);
			setCurrentRenderTarget((uint)m_hWnd);
		}
		else
		{
			//wglDeleteContext(m_pOverlayRenderTarget->rc);
			//ReleaseDC(m_hWnd,m_pOverlayRenderTarget->dc);
		}
		m_pOverlayRenderTarget->release();
		m_pOverlayRenderTarget = 0;
	}

	bool RenderSystem::setOverlayRenderTarget()
	{
		if(!m_pOverlayRenderTarget)return false;
		if(m_pCurrentRenderTarget == m_pOverlayRenderTarget)return true;

		if(m_pCurrentRenderTarget)m_pCurrentRenderTarget->onDetach();
		//wglMakeCurrent(m_pOverlayRenderTarget->dc,m_pOverlayRenderTarget->rc);
		m_pOverlayRenderTarget->onAttach();

		m_currentRenderTarget = 0;
		m_pCurrentRenderTarget = m_pOverlayRenderTarget;

		return true;
	}

	uint RenderSystem::getCurrentRenderTarget()
	{
		return m_currentRenderTarget;
	}

	bool RenderSystem::getRenderTargetTextureData(uint hwnd,void  *pData)
	{
		RenderTargetType::iterator it = m_vRenderTarget.find(hwnd);
		if(it == m_vRenderTarget.end())return false;

		return it->second->getTextureData(pData);
	}

	RenderTarget* RenderSystem::initializeRenderTarget(HWND hWnd,int layer)
	{
		Assert((layer == 0 || layer == 1));

#if (TARGET_PLATFORM == PLATFORM_WIN32)
		HDC hDC = GetDC(hWnd);
		if(m_pCurrentRenderTarget)
		{
			//wglShareLists(m_pCurrentRenderTarget->rc,hRC);
		}
		RenderTarget *pRenderTarget = new RenderTarget(hDC,hWnd,this);
#else
        RenderTarget* pRenderTarget = new RenderTarget(hWnd,this,m_pContext);
#endif
		return pRenderTarget;
	}


	bool	RenderSystem::create(RenderEngineCreationParameters *param)
	{
		if(param->rst != RS_OPENGLES2)return false;

		ShaderProgramManagerOGL::Instance()->m_pRenderSystem = this;

		HWND hWnd = (HWND)param->hwnd;
		if(!hWnd)
		{
			Trace("Trace:RenderSystem::create	m_hWnd == 0\n");
			return false;
		}
		m_hWnd = hWnd;
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		//初始化display
		//win32 下实现
		//---------------------
		EGLNativeWindowType eglWindow = hWnd;
		m_hDC = GetDC(hWnd);

		if (!m_hDC) 
			return false;

		m_eglDisplay = eglGetDisplay(m_hDC);
		if(m_eglDisplay == EGL_NO_DISPLAY)
			m_eglDisplay = eglGetDisplay((EGLNativeDisplayType) EGL_DEFAULT_DISPLAY);
		//TestGLError("eglGetDisplay");

		EGLint iMajorVersion, iMinorVersion;
		if (!eglInitialize(m_eglDisplay, &iMajorVersion, &iMinorVersion))
			return false;
		//TestGLError("eglInitialize");

		eglBindAPI(EGL_OPENGL_ES_API);
		//TestGLError("eglBindAPI");

		const EGLint pi32ConfigAttribs[] =
		{
			EGL_LEVEL,				0,
			EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
			EGL_NATIVE_RENDERABLE,	EGL_FALSE,
			//EGL_DEPTH_SIZE,			EGL_DONT_CARE,
			EGL_ALPHA_SIZE,         8,
			EGL_BLUE_SIZE,          8,
			EGL_GREEN_SIZE,         8,
			EGL_RED_SIZE,           8,
			EGL_DEPTH_SIZE,			16,
			EGL_STENCIL_SIZE,       8,
			EGL_NONE
		};
		EGLint iConfigs;
		if (!eglChooseConfig(m_eglDisplay, pi32ConfigAttribs, &m_eglConfig, 1, &iConfigs) || (iConfigs != 1))
		{
			return false;
		}
		m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, eglWindow, NULL);
		if(m_eglSurface == EGL_NO_SURFACE)
		{
			eglGetError(); // Clear error
			m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, NULL, NULL);
		}
		if(m_eglSurface == EGL_NO_SURFACE)
			return false;
		//-----------------------------------------
#else
        
#endif
		m_stencilBuffer = param->stencilBuffer ? 8 : 0;
		if(!addRenderTarget((uint)hWnd))return false;
		setCurrentRenderTarget((uint)hWnd);

#if (TARGET_PLATFORM == PLATFORM_WIN32)
		m_hWnd = m_pCurrentRenderTarget->m_hWnd;
		m_hDC = m_pCurrentRenderTarget->dc;
		m_hRC = m_pCurrentRenderTarget->rc;
#else
        m_hWnd = m_pCurrentRenderTarget->getView();
        m_pContext = m_pCurrentRenderTarget->getRenderContext();
#endif

		xs::Rect rc;
		m_pCurrentRenderTarget->getRect(&rc);
		setDefaultMatrix(rc.right - rc.left,rc.bottom - rc.top);

#if (TARGET_PLATFORM == PLATFORM_WIN32)
		if(param->fullscreen)
		{
			Adapter::Instance()->toggleMode(hWnd,param->fullscreen,param->w,param->h,param->colorDepth,param->refreshRate);
		}
#endif

		initCapabilities();

		/*glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
		glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);*/

		setVerticalSync(param->vertcialSync);
		setColor(m_pCurrentRenderTarget->m_RenderState.m_color);

		//GLint para;
		//glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,&para);
		//GL_REPEAT;
		//glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,&para);
		//GL_REPEAT;

		setCullingMode(CULL_CLOCKWISE);
		glPixelStorei( GL_UNPACK_ALIGNMENT,1);
		glPixelStorei( GL_PACK_ALIGNMENT,1);

		//glEnable(GL_SCISSOR_TEST);
		m_vRenderSystems.push_back(this);

		//----other init-------------------------------------
		m_pWhiteTex = TextureManager::Instance()->createEmptyTexture(DEFAULT_TEX_SIZE,DEFAULT_TEX_SIZE,PF_R8G8B8A8);
		unsigned char* buffer = new unsigned char[DEFAULT_TEX_SIZE * DEFAULT_TEX_SIZE *4];
		memset(buffer,0xff,DEFAULT_TEX_SIZE * DEFAULT_TEX_SIZE *4);
		m_pWhiteTex->loadFromRawData(buffer,DEFAULT_TEX_SIZE,DEFAULT_TEX_SIZE,PF_R8G8B8A8);
		delete [] buffer;

		if(!_createInnerShader())
			return false;

		return true;
	}

	bool RenderSystem::switchDisplayMode(RenderEngineCreationParameters* param)
	{
		RenderTargetType::iterator it = m_vRenderTarget.find(m_currentRenderTarget);
		if(it == m_vRenderTarget.end())return false;
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		return Adapter::Instance()->toggleMode((void *)(it->first),param->fullscreen,param->w,param->h,param->colorDepth,param->refreshRate);
#else
        return true;
#endif
	}

	void RenderSystem::getViewport(int &left,int &top,int &width,int &height)
	{
		/*GLint vp[4];
		glGetIntegerv(GL_VIEWPORT,vp);
		left = vp[0];
		top = vp[1];
		width = vp[2];
		height = vp[3];*/
		left = m_pCurrentRenderTarget->m_vpLeft;
		top = m_pCurrentRenderTarget->m_vpTop;
		width = m_pCurrentRenderTarget->m_vpWidth;
		height = m_pCurrentRenderTarget->m_vpHeight;
	}

	void RenderSystem::setViewport(int left,int top,int width,int height)
	{
		xs::Rect rc;
		m_pCurrentRenderTarget->getRect(&rc);

		int h = rc.bottom - rc.top;
        #if 0
		m_pCurrentRenderTarget->m_vpLeft = left;
		m_pCurrentRenderTarget->m_vpTop = h - top - height;
		m_pCurrentRenderTarget->m_vpWidth = width;
		m_pCurrentRenderTarget->m_vpHeight = height;
        #else
        m_pCurrentRenderTarget->m_vpLeft = 0;
		m_pCurrentRenderTarget->m_vpTop = 0;
		m_pCurrentRenderTarget->m_vpWidth = rc.right;
		m_pCurrentRenderTarget->m_vpHeight = rc.bottom;
        #endif
		if(m_scissorEnabled)
			glScissor(m_pCurrentRenderTarget->m_vpLeft,m_pCurrentRenderTarget->m_vpTop,m_pCurrentRenderTarget->m_vpWidth,m_pCurrentRenderTarget->m_vpHeight);
		
		glViewport(m_pCurrentRenderTarget->m_vpLeft,m_pCurrentRenderTarget->m_vpTop,m_pCurrentRenderTarget->m_vpWidth,m_pCurrentRenderTarget->m_vpHeight);

		if(m_pCurrentRenderTarget->m_b2D)
		{
			Matrix4 mtxW = Matrix4::IDENTITY;
			mtxW[1][1] = -1;
			mtxW[1][3] = rc.bottom - rc.top;
			setViewMatrix(mtxW);
			setProjectionMatrix(Matrix4::orthogonalProjection(m_pCurrentRenderTarget->m_vpLeft,m_pCurrentRenderTarget->m_vpLeft + m_pCurrentRenderTarget->m_vpWidth,m_pCurrentRenderTarget->m_vpTop,m_pCurrentRenderTarget->m_vpTop + m_pCurrentRenderTarget->m_vpHeight,-1,1));
		}
	}

	void RenderSystem::setClearColor(const ColorValue& color)
	{
		m_pCurrentRenderTarget->m_RenderState.m_clearColor = color;
		glClearColor(color.r,color.g,color.b,color.a);
	}

	const ColorValue& RenderSystem::getClearColor()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_clearColor;
	}

	void RenderSystem::setClearDepth(float depth)
	{
		glClearDepthf(depth);
	}

	void 		RenderSystem::setClearStencil(int stencil)
	{
		glClearStencil(stencil);
	}


	bool RenderSystem::getDisplayMode(RenderEngineCreationParameters & createParam)
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		return Adapter::Instance()->getCurrentMode( createParam.hwnd, 
			createParam.fullscreen, 
			createParam.w, 
			createParam.h,
			createParam.colorDepth,
			createParam.refreshRate);
#else
        createParam.fullscreen = true;
        xs::Rect rc;
        m_pCurrentRenderTarget->GetClientRect(&rc);
        createParam.w = rc.right - rc.left;
        createParam.h = rc.bottom - rc.top;
        createParam.colorDepth = 1;
        createParam.refreshRate = 60;
        return true;
#endif
	}

	void RenderSystem::clearFrameBuffer(bool clearColorBuffer,bool clearDepthBuffer,bool clearStencilBuffer)
	{
		GLbitfield mask = 0;
		if(clearDepthBuffer)
			mask |= GL_DEPTH_BUFFER_BIT;
		if(clearStencilBuffer)
			mask |= GL_STENCIL_BUFFER_BIT;
		if(clearColorBuffer)
			mask |= GL_COLOR_BUFFER_BIT;

		glClear(mask);
	}
	void RenderSystem::beginFrame(bool clearColorBuffer,bool clearDepthBuffer,bool clearStencilBuffer)
	{
		clearFrameBuffer(clearColorBuffer,clearDepthBuffer,clearStencilBuffer);
	}

	void RenderSystem::endFrame(int layer)
	{
		m_pCurrentRenderTarget->onEndFrame(layer);
	}

	void  RenderSystem::setRenderState(RenderTarget::RenderState *pRenderState)
	{
		setViewMatrix(pRenderState->m_mtxView);
		setWorldMatrix(pRenderState->m_mtxWorld);
		setProjectionMatrix(pRenderState->m_mtxProjection);
		setDepthBufferWriteEnabled(pRenderState->m_bDepthWriteEnabled);
		setLightingEnabled(pRenderState->m_bLightingEnabled);
		setSceneBlending(pRenderState->m_BlendFactorSource,pRenderState->m_BlendFactorDest);
		setDepthBufferCheckEnabled(pRenderState->m_bDepthCheckEnabled);
		setCullingMode(pRenderState->m_cullingMode);
		setColor(pRenderState->m_color);
		setAlphaCheckEnabled(pRenderState->m_bAlphaCheckEnabled);
		setAlphaFunction(pRenderState->m_AlphaCompareFunction,pRenderState->m_AlphaCompareValue);
		setFillMode(pRenderState->m_fillMode);
		setAmbientLight(pRenderState->m_ambientColor);
		setClearColor(pRenderState->m_clearColor);
		setDepthBufferFunction(pRenderState->m_DepthBufferFunction);
	}

	void		RenderSystem::switchTo2D()
	{
		if(m_pCurrentRenderTarget->m_b2D)return;
		m_pCurrentRenderTarget->m_b2D = true;
		m_pCurrentRenderTarget->m_RenderState3D = m_pCurrentRenderTarget->m_RenderState;

		xs::Rect rc;
		m_pCurrentRenderTarget->getRect(&rc);

		Matrix4 mtxW = Matrix4::IDENTITY;
		mtxW[1][1] = -1;
		mtxW[1][3] = rc.bottom - rc.top;
		m_pCurrentRenderTarget->m_RenderState2D.m_mtxView = mtxW;

		setRenderState(&m_pCurrentRenderTarget->m_RenderState2D);

		setProjectionMatrix(Matrix4::orthogonalProjection(m_pCurrentRenderTarget->m_vpLeft,m_pCurrentRenderTarget->m_vpLeft + m_pCurrentRenderTarget->m_vpWidth,m_pCurrentRenderTarget->m_vpTop,m_pCurrentRenderTarget->m_vpTop + m_pCurrentRenderTarget->m_vpHeight,-1,1));
	}

	void 		RenderSystem::switchTo2D(int left,int top,int width,int height)
	{
		setViewport(left,top,width,height);
		switchTo2D();
	}

	void		RenderSystem::switchTo3D()
	{
		if(!m_pCurrentRenderTarget->m_b2D)return;
		m_pCurrentRenderTarget->m_b2D = false;
		m_pCurrentRenderTarget->m_RenderState2D = m_pCurrentRenderTarget->m_RenderState;
		setRenderState(&m_pCurrentRenderTarget->m_RenderState3D);
	}

	bool		RenderSystem::is2dMode()
	{
		return m_pCurrentRenderTarget->m_b2D;
	}

	void		RenderSystem::setProjectionMatrix(const Matrix4& mtx)
	{
		m_pCurrentRenderTarget->m_RenderState.m_mtxProjection = mtx;
		/*
		glMatrixMode(GL_PROJECTION);
		Matrix4 m = mtx.transpose();
		glLoadMatrixf(&m[0][0]);
		glMatrixMode(GL_MODELVIEW);
		*/
		_setWorldViewProj2Shader();
	}

	void		RenderSystem::setViewMatrix(const Matrix4& mtx)
	{
		m_pCurrentRenderTarget->m_RenderState.m_mtxView = mtx;
		m_pCurrentRenderTarget->m_RenderState.m_mtxModelView = m_pCurrentRenderTarget->m_RenderState.m_mtxView * m_pCurrentRenderTarget->m_RenderState.m_mtxWorld;
		
		//Matrix4 m = m_pCurrentRenderTarget->m_RenderState.m_mtxModelView.transpose();
		//glLoadMatrixf(&m[0][0]);
		_setWorldViewProj2Shader();
	}

	const Matrix4&	RenderSystem::getProjectionMatrix()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_mtxProjection;
	}

	const Matrix4&	RenderSystem::getViewMatrix()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_mtxView;
	}

	const Matrix4&	RenderSystem::getWorldMatrix()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_mtxWorld;
	}
	
	const Matrix4&		RenderSystem::getModelViewMatrix()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_mtxModelView;
	}

	void		RenderSystem::setWorldMatrix(const Matrix4 &mtx)
	{
		m_pCurrentRenderTarget->m_RenderState.m_mtxWorld = mtx;
		m_pCurrentRenderTarget->m_RenderState.m_mtxModelView = m_pCurrentRenderTarget->m_RenderState.m_mtxView * m_pCurrentRenderTarget->m_RenderState.m_mtxWorld;
		
		//Matrix4 m = m_pCurrentRenderTarget->m_RenderState.m_mtxModelView.transpose();
		//glLoadMatrixf(&m[0][0]);

		_setWorldViewProj2Shader();
	}

	void		RenderSystem::setDepthBufferWriteEnabled(bool enabled)
	{
		GLboolean flag = enabled ? GL_TRUE : GL_FALSE;
		if(m_pCurrentRenderTarget->m_RenderState.m_bDepthWriteEnabled != enabled)
		{
			glDepthMask(flag);
			m_pCurrentRenderTarget->m_RenderState.m_bDepthWriteEnabled = enabled;
		}
	}

	bool		RenderSystem::isDepthBufferWriteEnabled()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_bDepthWriteEnabled;
	}

	void		RenderSystem::setLightingEnabled(bool enabled)
	{
		if(m_pCurrentRenderTarget->m_RenderState.m_bLightingEnabled != enabled)
		{
			/*
			if (enabled) 
			{      
				glEnable(GL_LIGHTING);
			} 
			else 
			{
				glDisable(GL_LIGHTING);
			}
			*/
			m_pCurrentRenderTarget->m_RenderState.m_bLightingEnabled = enabled;
		}
	}

	bool		RenderSystem::isLightingEnabled()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_bLightingEnabled;
	}

	const FillMode&	RenderSystem::getFillMode()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_fillMode;
	}

	void		RenderSystem::setFillMode(const FillMode& sdl)
	{
		if(m_pCurrentRenderTarget->m_RenderState.m_fillMode == sdl)return;

		m_pCurrentRenderTarget->m_RenderState.m_fillMode = sdl;
		
		assert(0);
		/*
		GLenum glmode;
		switch(sdl)
		{
		case FM_POINTS:
			glmode = GL_POINT;
			break;
		case FM_WIREFRAME:
			glmode = GL_LINE;
			break;
		case FM_SOLID:
			glmode = GL_FILL;
			break;
		}
		glPolygonMode(GL_FRONT_AND_BACK,glmode);
		*/
	}

	void RenderSystem::setLineWidth(float width)
	{
		m_lineWidth = width;
		glLineWidth(width);
	}

	float RenderSystem::getLineWidth()
	{
		return m_lineWidth;
	}

	bool		RenderSystem::isDepthBufferCheckEnabled()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_bDepthCheckEnabled;
	}

	void RenderSystem::setDepthBufferCheckEnabled(bool enabled)
	{
		if(m_pCurrentRenderTarget->m_RenderState.m_bDepthCheckEnabled != enabled)
		{
			if (enabled)
			{
				glClearDepthf(1.0f);
				glEnable(GL_DEPTH_TEST);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}
			m_pCurrentRenderTarget->m_RenderState.m_bDepthCheckEnabled = enabled;
		}
	}
	//---------------------------------------------------------------------
	GLint RenderSystem::convertCompareFunction(CompareFunction func) const
	{
		switch(func)
		{
		case CMPF_ALWAYS_FAIL:
			return GL_NEVER;
		case CMPF_ALWAYS_PASS:
			return GL_ALWAYS;
		case CMPF_LESS:
			return GL_LESS;
		case CMPF_LESS_EQUAL:
			return GL_LEQUAL;
		case CMPF_EQUAL:
			return GL_EQUAL;
		case CMPF_NOT_EQUAL:
			return GL_NOTEQUAL;
		case CMPF_GREATER_EQUAL:
			return GL_GEQUAL;
		case CMPF_GREATER:
			return GL_GREATER;
		};
		// to keep compiler happy
		return GL_ALWAYS;
	}

	void RenderSystem::setAlphaFunction(const CompareFunction& cf,float value)
	{
		m_pCurrentRenderTarget->m_RenderState.m_AlphaCompareFunction = cf;
		m_pCurrentRenderTarget->m_RenderState.m_AlphaCompareValue = value;
		
		//assert(0);
		//glAlphaFunc(convertCompareFunction(cf),value);
	}

	void RenderSystem::setAlphaFunction(const CompareFunction& cf,uchar value)
	{
		//glAlphaFunc(convertCompareFunction(cf),value / 255.0f);
		setAlphaFunction(cf,value / 255.0f);
	}

	void		RenderSystem::getAlphaFunction(CompareFunction& cf,float& value)
	{
		cf = m_pCurrentRenderTarget->m_RenderState.m_AlphaCompareFunction;
		value = m_pCurrentRenderTarget->m_RenderState.m_AlphaCompareValue;
	}

	bool		RenderSystem::alphaCheckEnabled()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_bAlphaCheckEnabled;
	}

	void RenderSystem::setAlphaCheckEnabled(bool bEnabled)
	{
		if(m_pCurrentRenderTarget->m_RenderState.m_bAlphaCheckEnabled != bEnabled)
		{
			m_pCurrentRenderTarget->m_RenderState.m_bAlphaCheckEnabled = bEnabled;
			
			assert(0);
			//bEnabled ? glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);
		}
	}

	void RenderSystem::setDepthBufferFunction(const CompareFunction& cf)
	{
		if(m_pCurrentRenderTarget->m_RenderState.m_DepthBufferFunction != cf)
		{
			glDepthFunc(convertCompareFunction(cf));
			m_pCurrentRenderTarget->m_RenderState.m_DepthBufferFunction = cf;
		}
	}
	
	const CompareFunction& RenderSystem::getDepthBufferFunction()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_DepthBufferFunction;
	}
	
	const CullingMode& RenderSystem::getCullingMode()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_cullingMode;
	}
	
	void RenderSystem::setEdgeFlag(bool bEdgeFlag)
	{
		assert(0);
		//glEdgeFlag(bEdgeFlag);
	}

	void RenderSystem::setCullingMode(const CullingMode& cm)
	{
		if(m_pCurrentRenderTarget->m_RenderState.m_cullingMode != cm)
		{
			m_pCurrentRenderTarget->m_RenderState.m_cullingMode = cm;

			GLenum cullMode;

			switch(cm)
			{
			case CULL_NONE:
				glDisable(GL_CULL_FACE);
				return;
			case CULL_CLOCKWISE:
				cullMode = GL_BACK;
				break;
			case CULL_COUNTERCLOCKWISE:
				cullMode = GL_FRONT;
				break;
			}

			glEnable(GL_CULL_FACE);
			glCullFace(cullMode);
		}
	}
	//-----------------------------------------------------------------------------
	GLint RenderSystem::getBlendMode(SceneBlendFactor blend) const
	{
		switch(blend)
		{
		case SBF_ONE:
			return GL_ONE;
		case SBF_ZERO:
			return GL_ZERO;
		case SBF_DEST_COLOR:
			return GL_DST_COLOR;
		case SBF_SOURCE_COLOR:
			return GL_SRC_COLOR;
		case SBF_ONE_MINUS_DEST_COLOR:
			return GL_ONE_MINUS_DST_COLOR;
		case SBF_ONE_MINUS_SOURCE_COLOR:
			return GL_ONE_MINUS_SRC_COLOR;
		case SBF_DEST_ALPHA:
			return GL_DST_ALPHA;
		case SBF_SOURCE_ALPHA:
			return GL_SRC_ALPHA;
		case SBF_ONE_MINUS_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case SBF_ONE_MINUS_SOURCE_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		};
		// to keep compiler happy
		return GL_ONE;
	}

	void RenderSystem::getSceneBlending(SceneBlendFactor& src,SceneBlendFactor& dst)
	{
		src = m_pCurrentRenderTarget->m_RenderState.m_BlendFactorSource;
		dst = m_pCurrentRenderTarget->m_RenderState.m_BlendFactorDest;
	}

	void RenderSystem::setSceneBlending(const SceneBlendFactor& sourceFactor,const SceneBlendFactor& destFactor)
	{
		if(m_pCurrentRenderTarget->m_RenderState.m_BlendFactorSource == sourceFactor && 
			m_pCurrentRenderTarget->m_RenderState.m_BlendFactorDest == destFactor)return;

		GLint sourceBlend = getBlendMode(sourceFactor);
		GLint destBlend = getBlendMode(destFactor);
		if(sourceFactor == SBF_ONE && destFactor == SBF_ZERO)
		{
			glDisable(GL_BLEND);
		}
		else
		{
			glEnable(GL_BLEND);
			glBlendFunc(sourceBlend,destBlend);
		}
		m_pCurrentRenderTarget->m_RenderState.m_BlendFactorSource = sourceFactor;
		m_pCurrentRenderTarget->m_RenderState.m_BlendFactorDest = destFactor;
	}

	IBufferManager*		RenderSystem::getBufferManager()
	{
		//return m_rsc.hasCapability(RSC_VBO) ? BufferManager::Instance() : DefaultBufferManager::Instance();
		return BufferManager::Instance();
	}

	ITextureManager*		RenderSystem::getTextureManager()
	{
		return TextureManager::Instance();
	}

	TextureAddressingMode	RenderSystem::getTextureWrapS(ushort unit)
	{
		GLint mode;
		if(unit)glActiveTexture(GL_TEXTURE0 + unit);
		glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,&mode);
		if(unit)glActiveTexture(GL_TEXTURE0);

		return getAddressingMode(mode);
	}

	TextureAddressingMode	RenderSystem::getTextureWrapT(ushort unit)
	{
		GLint mode;
		if(unit)glActiveTexture(GL_TEXTURE0 + unit);
		glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,&mode);
		if(unit)glActiveTexture(GL_TEXTURE0);

		return getAddressingMode(mode);
	}

	void RenderSystem::setTextureWrapS(ushort unit,const TextureAddressingMode& tam)
	{
		if(unit)glActiveTexture(GL_TEXTURE0 + unit);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,getAddressingMode(tam));
		if(unit)glActiveTexture(GL_TEXTURE0);
	}

	void RenderSystem::setTextureWrapT(ushort unit,const TextureAddressingMode& tam)
	{
		if(unit)glActiveTexture(GL_TEXTURE0 + unit);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,getAddressingMode(tam));
		if(unit)glActiveTexture(GL_TEXTURE0);
	}

	void RenderSystem::setTextureCoordCalculation(ushort unit,TexCoordCalcMethod m)
	{
		//assert(0);
		/*
		if(unit)glActiveTexture(GL_TEXTURE0 + unit);
		switch(m)
		{
		case TEXCALC_NONE:
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			break;
		case TEXCALC_ENVIRONMENT_MAP:
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
			glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
			break;
		}
		if(unit)glActiveTexture(GL_TEXTURE0);
		*/
	}

	void RenderSystem::setTextureWrap(ushort unit,const TextureAddressingMode& tamS,const TextureAddressingMode& tamT)
	{
		if(unit)glActiveTexture(GL_TEXTURE0 + unit);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,getAddressingMode(tamS));
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,getAddressingMode(tamT));
		if(unit)glActiveTexture(GL_TEXTURE0);
	}

	IShaderProgramManager* RenderSystem::getShaderProgramManager()
	{
		return ShaderProgramManagerOGL::Instance();
	}

	void 		RenderSystem::setMultiTexcoord(ushort unit,const Vector2& texcoord)
	{
		if(unit == 0)
		{
			_checkCacheAndFlush(m_batchStatus.texCoord0Count);
			m_batchStatus.flagBits.texCoord0 = true;
			m_batchStatus.texCoord0s[m_batchStatus.texCoord0Count++] = texcoord;
		}
		else if(unit == 1)
		{
			_checkCacheAndFlush(m_batchStatus.texCoord1Count);
			m_batchStatus.flagBits.texCoord1 = true;
			m_batchStatus.texCoord1s[m_batchStatus.texCoord1Count++] = texcoord;
		}
		else
		{
			printf("\n暂时支持2层纹理!!");
		}
		//glMultiTexCoord2f(GL_TEXTURE0 + unit,texcoord.x,texcoord.y);
	}

	void		RenderSystem::setTexcoord(const Vector2& texcoord)
	{
		_checkCacheAndFlush(m_batchStatus.texCoord0Count);
		m_batchStatus.flagBits.texCoord0 = true;
		m_batchStatus.texCoord0s[m_batchStatus.texCoord0Count++] = texcoord;
		
		//glTexCoord2f(texcoord.x,texcoord.y);
	}

	void		RenderSystem::setColor(const ColorValue& color)
	{
		m_pCurrentRenderTarget->m_RenderState.m_color = color;
		
		//glColor4f(color.r,color.g,color.b,color.a);

		if(m_batchStatus.isPrimitiveing)
		{
			_checkCacheAndFlush(m_batchStatus.colorCount);
			m_batchStatus.colors[m_batchStatus.colorCount++] = color.getAsABGR();
		}
		else
		{
			/*
			if(m_pCurrentShaderProgram)
			{
				Vector4 c(color.r,color.g,color.b,color.a);
				m_pCurrentShaderProgram->setUniformVector4(G_DIFFUSE,c);
			}
			*/
			IHighLevelShaderProgram* pShader = static_cast<IHighLevelShaderProgram*>(getShaderProgramManager()->getCurrentShaderProgam());
			if(pShader)
			{
				//Vector4 c(color.r,color.g,color.b,color.a);
				//pShader->setUniformVector4(G_DIFFUSE,c);
				pShader->bindTransformMatrix(TMT_COLOR);
			}
		}
	}

	const ColorValue&		RenderSystem::getColor()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_color;
	}

	void		RenderSystem::setNormal(const Vector3& normal)
	{
		//glNormal3fv(&normal.x);

		_checkCacheAndFlush(m_batchStatus.normalCount);
		m_batchStatus.flagBits.normal = true;
		m_batchStatus.normals[m_batchStatus.normalCount++] = normal;
	}

	void		RenderSystem::sendVertex(const Vector2& vertex)
	{
		//glVertex2fv(&vertex.x);

		_checkCacheAndFlush(m_batchStatus.vertexCount);
		m_batchStatus.flagBits.vertex = true;
		m_batchStatus.vertexs[m_batchStatus.vertexCount].x = vertex.x;
		m_batchStatus.vertexs[m_batchStatus.vertexCount].y = vertex.y;
		m_batchStatus.vertexs[m_batchStatus.vertexCount].z = 0;
		++m_batchStatus.vertexCount;
	}

	void		RenderSystem::sendVertex(const Vector3& vertex)
	{
		//glVertex3fv(&vertex.x);

		_checkCacheAndFlush(m_batchStatus.vertexCount);
		m_batchStatus.flagBits.vertex = true;
		m_batchStatus.vertexs[m_batchStatus.vertexCount++] = vertex;
	}

	void		RenderSystem::sendVertex(const Vector4& vertex)
	{
		assert(0);
		//glVertex4fv(&vertex.x);
	}

	void		RenderSystem::beginPrimitive(PrimitiveType pt)
	{
		//glBegin(getPrimitiveType(pt));

		m_batchStatus.pt = pt;
		m_batchStatus.isPrimitiveing = true;
		m_batchStatus.clear();

		setIndexBuffer(0);
		//setVertexVertexBuffer(0);
		setDiffuseVertexBuffer(0);
		setNormalVertexBuffer(0);
		setTexcoordVertexBuffer(0,0);
		setTexcoordVertexBuffer(1,0);

		//vertex pos and color always

		//bind shader
		//bindCurrentShaderProgram(getShaderProgram(ESP_V3_UV_C));
		IHighLevelShaderProgram* pShader = getShaderProgram(ESP_V3_UV_C);
		if(pShader)
		{
			pShader->bind();
		}
	}

	void		RenderSystem::endPrimitive()
	{
		//glEnd();

		_flush();

		m_batchStatus.isPrimitiveing = false;

		//disable all except vetext pos
		setVertexVertexBuffer(0);
		setIndexBuffer(0);
		setDiffuseVertexBuffer(0);
		setNormalVertexBuffer(0);
		setTexcoordVertexBuffer(0,0);
		setTexcoordVertexBuffer(1,0);
	}

	void		RenderSystem::render(const RenderOperation& op)
	{
		assert(0);

		/*
		void*	pBufferData = 0;
		bool	bHasVBO = m_rsc.hasCapability(RSC_VBO);

		const VertexDeclaration::VertexElementList& decl = op.vertexData->vertexDeclaration->getElements();
		VertexDeclaration::VertexElementList::const_iterator elem,elemEnd;
		elemEnd = decl.end();

		for (elem = decl.begin();elem != elemEnd;++elem)
		{
			const VertexElement& element = (*elem);

			uint ui32Offset = element.getOffset();
			VertexElementType	eType = element.getType();

			IVertexBuffer* vertexBuffer = op.vertexData->vertexBufferBinding->getBuffer(element.getSource());
			if(!vertexBuffer)continue;

			uint ui32VertexBufferSize = vertexBuffer->getVertexSize();
			ushort ui16TypeCount = VertexElement::getTypeCount(eType);
			GLenum eGLType = BufferManager::getGLType(eType);

			if(bHasVBO)
			{
				glBindBuffer(GL_ARRAY_BUFFER,static_cast<VertexBuffer*>(vertexBuffer)->getGLBufferId());
				pBufferData = VBO_BUFFER_OFFSET(ui32Offset);
			}
			else
			{
				pBufferData = static_cast<DefaultVertexBuffer*>(vertexBuffer)->getDataPtr(ui32Offset);
			}
			if (op.vertexData->vertexStart)
			{
				pBufferData = static_cast<uchar*>(pBufferData) + op.vertexData->vertexStart * vertexBuffer->getVertexSize();
			}

			uint i = 0;

			switch(element.getSemantic())
			{
			case VES_POSITION:
				glVertexPointer(ui16TypeCount,eGLType,static_cast<GLsizei>(ui32VertexBufferSize),pBufferData);
				glEnableClientState(GL_VERTEX_ARRAY);
				break;
			case VES_NORMAL:
				glNormalPointer(eGLType,static_cast<GLsizei>(ui32VertexBufferSize),pBufferData);
				glEnableClientState(GL_NORMAL_ARRAY);
				break;
			case VES_DIFFUSE:
				glColorPointer(4,eGLType,static_cast<GLsizei>(ui32VertexBufferSize),pBufferData);
				glEnableClientState(GL_COLOR_ARRAY);
				break;
			case VES_SPECULAR:
				glSecondaryColorPointer(4,eGLType,static_cast<GLsizei>(ui32VertexBufferSize),pBufferData);
				glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
				break;
			case VES_TEXTURE_COORDINATES:
				for (i = 0;i < RE_MAX_TEXTURE_COORD_SETS;i++)
				{
					if (i == element.getIndex())
					{
						glClientActiveTexture(GL_TEXTURE0 + i);
						glTexCoordPointer(ui16TypeCount,eGLType,
									static_cast<GLsizei>(ui32VertexBufferSize),pBufferData);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);

						break;
					}
				}
				break;
			case VES_BLEND_INDICES:
				assert(m_rsc.hasCapability(RSC_VERTEX_PROGRAM));
				glVertexAttribPointer(
					7,											// matrix indices are vertex attribute 7 (no def?)
					ui16TypeCount,
					eGLType,
					GL_FALSE,									// normalisation disabled
					static_cast<GLsizei>(ui32VertexBufferSize),
					pBufferData);
				glEnableVertexAttribArray(7);
				break;
			case VES_BLEND_WEIGHTS:
				assert(m_rsc.hasCapability(RSC_VERTEX_PROGRAM));
				glVertexAttribPointer(
					1,											// weights are vertex attribute 1 (no def?)
					ui16TypeCount, 
					eGLType, 
					GL_FALSE,									// normalisation disabled
					static_cast<GLsizei>(ui32VertexBufferSize), 
					pBufferData);
				glEnableVertexAttribArray(1);
				break;
			default:
				break;
			};
		}

		// Find the correct type to render
		GLint primType = getPrimitiveType(op.primitiveType);

		if (op.bUseIndices)
		{
			if(bHasVBO)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
					static_cast<IndexBuffer*>(op.indexData->indexBuffer)->getGLBufferId());

				pBufferData = VBO_BUFFER_OFFSET(
					op.indexData->indexStart * op.indexData->indexBuffer->getIndexSize());
			}
			else
			{
				pBufferData = static_cast<DefaultIndexBuffer*>(op.indexData->indexBuffer)->
						getDataPtr(op.indexData->indexStart * op.indexData->indexBuffer->getIndexSize());
			}

			GLenum indexType = (op.indexData->indexBuffer->getType() == IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
			glDrawElements(primType,op.indexData->indexCount,indexType,pBufferData);
		}
		else
		{
			glDrawArrays(primType,0,op.vertexData->vertexCount);
		}

		if(bHasVBO)
		{
			glBindBuffer(GL_ARRAY_BUFFER,0);
			if(op.bUseIndices)glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		}

		glDisableClientState(GL_VERTEX_ARRAY);
		for(uint i = 0; i < RE_MAX_TEXTURE_COORD_SETS;i++)
		{
			glClientActiveTexture(GL_TEXTURE0 + i);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		glClientActiveTexture(GL_TEXTURE0);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_SECONDARY_COLOR_ARRAY);

		if (m_rsc.hasCapability(RSC_VERTEX_PROGRAM))
		{
			glDisableVertexAttribArray(7); // disable indices
			glDisableVertexAttribArray(1); // disable weights
		}
		*/
	}
	
	/*
	GLenum getGLVertexFormat(VertexFormat format)
	{
		switch(format)
		{
		case VF_V2F:
			return GL_V2F;
		case VF_V3F:
			return GL_V3F;
		case VF_C4UB_V2F:
			return GL_C4UB_V2F;
		case VF_C4UB_V3F:
			return GL_C4UB_V3F;
		case VF_C3F_V3F:
			return GL_C3F_V3F;
		case VF_N3F_V3F:
			return GL_N3F_V3F;
		case VF_C4F_N3F_V3F:
			return GL_C4F_N3F_V3F;
		case VF_T2F_V3F:
			return GL_T2F_V3F;
		case VF_T4F_V4F:
			return GL_T4F_V4F;
		case VF_T2F_C4UB_V3F:
			return GL_T2F_C4UB_V3F;
		case VF_T2F_C3F_V3F:
			return GL_T2F_C3F_V3F;
		case VF_T2F_N3F_V3F:
			return GL_T2F_N3F_V3F;
		case VF_T2F_C4F_N3F_V3F:
			return GL_T2F_C4F_N3F_V3F;
		case VF_T4F_C4F_N3F_V4F:
			return GL_T4F_C4F_N3F_V4F;
		}
		//should never get here
		return VF_V2F;
	}
	*/

	void		RenderSystem::render(const RenderDirect& rd)
	{
		assert(0);

		/*
		glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

		glInterleavedArrays(getGLVertexFormat(rd.vertexData.vertexFormat),0,rd.vertexData.pVertexData);
		if(rd.bUseIndices)
		{
			GLenum indexType = (rd.indexData.indexType == IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
			glDrawElements(getPrimitiveType(rd.primitiveType),rd.indexData.count,indexType,rd.indexData.pIndexData);
		}
		else
		{
			glDrawArrays(getPrimitiveType(rd.primitiveType),0,rd.vertexData.count);
		}

		glPopClientAttrib();
		*/
	}

	void RenderSystem::_setTempTexture()
	{
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		Texture * pTextureImp =static_cast<Texture *>(m_pWhiteTex);
		glBindTexture(GL_TEXTURE_2D,pTextureImp->getGLTextureID());
	}

	void RenderSystem::setTexture(ushort unit,ITexture* pTexture)
	{
		

		
		//modified by xxh 由于压缩纹理的同步加载
		if(m_textureUnit != unit)glActiveTexture(GL_TEXTURE0 + unit);

		Texture * pTextureImp =static_cast<Texture *>(pTexture);

		if( pTextureImp && pTextureImp->uploadTextureToGraphicMemory() )
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, pTextureImp->getGLTextureID() );
		}
		else
		{
			//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_TEXTURE_2D);
		}
		m_textureUnit = unit;
	}

	const ColorValue& RenderSystem::getAmbientLight()
	{
		return m_pCurrentRenderTarget->m_RenderState.m_ambientColor;
	}

	void		RenderSystem::setAmbientLight(const ColorValue& color)
	{
		m_pCurrentRenderTarget->m_RenderState.m_ambientColor = color;
		Vector4 ambient(color.r,color.g,color.b,color.a);
		
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT,&ambient[0]);
	}

	void		RenderSystem::setTexcoordVertexBuffer(ushort unit,IVertexBuffer* vertexBuffer,uint start,uint stride)
	{
		VetextAttr type = (VetextAttr)(EVA_TEX_COORDS_0+unit);
		if(type > EVA_TEX_COORDS_1)
		{
			printf("\n暂时只支持2组纹理坐标!");
			return;
		}
		const AttrInfo* pInfo = getAttrInfo(type);
		GLint location = getAttrLocation(pInfo->usage,pInfo->index);
		if(!vertexBuffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER,0);
			glDisableVertexAttribArray(location);
			return;
		}

		VertexBuffer* pVb = static_cast<VertexBuffer*>(vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER,pVb->getGLBufferId());

		GLenum glType;
		GLint components;
		GLboolean normalized;
		getAttrGLInfo(pInfo->type,glType,components,normalized);

		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location
			, components
			, glType
			, normalized
			, stride
			, (const void*)(((char*)start))
			);
		TestGLError("setTexcoordVertexBuffer");
		/*
		if(!vertexBuffer)
		{
			if(unit)glClientActiveTexture(GL_TEXTURE0 + unit);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			if(unit)glClientActiveTexture(GL_TEXTURE0);
		}
		else
		{
			uint ui32VertexBufferSize = vertexBuffer->getVertexSize();
			void*	pBufferData = 0;
			bool	bHasVBO = m_rsc.hasCapability(RSC_VBO);

			if(bHasVBO)
			{
				glBindBuffer(GL_ARRAY_BUFFER,static_cast<VertexBuffer*>(vertexBuffer)->getGLBufferId());
				pBufferData = VBO_BUFFER_OFFSET(start);
			}
			else
			{
				pBufferData = static_cast<DefaultVertexBuffer*>(vertexBuffer)->getDataPtr(start);
			}

			if(unit)glClientActiveTexture(GL_TEXTURE0 + unit);

			glTexCoordPointer(2,GL_FLOAT,ui32VertexBufferSize,pBufferData);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			if(bHasVBO)glBindBuffer(GL_ARRAY_BUFFER,0);

			if(unit)glClientActiveTexture(GL_TEXTURE0);
		}
		*/
	}

	void		RenderSystem::setVertexVertexBuffer(IVertexBuffer* vertexBuffer,uint start,uint stride)
	{
		const AttrInfo* pInfo = getAttrInfo(EVA_POSITION);
		GLint location = getAttrLocation(pInfo->usage,pInfo->index);
		if(!vertexBuffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER,0);
			glDisableVertexAttribArray(location);
			return;
		}

		VertexBuffer* pVb = static_cast<VertexBuffer*>(vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER,pVb->getGLBufferId());

		GLenum glType;
		GLint components;
		GLboolean normalized;
		getAttrGLInfo(pInfo->type,glType,components,normalized);

		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location
			, components
			, glType
			, normalized
			, stride
			, (const void*)(((char*)start))
			);
		TestGLError("setVertexVertexBuffer");

		/*
		if(!vertexBuffer)
		{
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		else
		{
			uint ui32VertexBufferSize = vertexBuffer->getVertexSize();
			void*	pBufferData = 0;
			bool	bHasVBO = m_rsc.hasCapability(RSC_VBO);

			if(bHasVBO)
			{
				glBindBuffer(GL_ARRAY_BUFFER,static_cast<VertexBuffer*>(vertexBuffer)->getGLBufferId());
				pBufferData = VBO_BUFFER_OFFSET(start);
			}
			else
			{
				pBufferData = static_cast<DefaultVertexBuffer*>(vertexBuffer)->getDataPtr(start);
			}

			glVertexPointer(3,GL_FLOAT,ui32VertexBufferSize,pBufferData);
			glEnableClientState(GL_VERTEX_ARRAY);
			if(bHasVBO)glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		*/
	}

	void		RenderSystem::setDiffuseVertexBuffer(IVertexBuffer* vertexBuffer,uint start,uint stride)
	{
		const AttrInfo* pInfo = getAttrInfo(EVA_COLOR);
		GLint location = getAttrLocation(pInfo->usage,pInfo->index);
		if(!vertexBuffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER,0);
			glDisableVertexAttribArray(location);
			return;
		}

		VertexBuffer* pVb = static_cast<VertexBuffer*>(vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER,pVb->getGLBufferId());

		GLenum glType;
		GLint components;
		GLboolean normalized;
		getAttrGLInfo(pInfo->type,glType,components,normalized);

		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location
			, components
			, glType
			, normalized
			, stride
			, (const void*)(((char*)start))
			);
		TestGLError("setDiffuseVertexBuffer");
		/*
		if(!vertexBuffer)
		{
			glDisableClientState(GL_COLOR_ARRAY);
		}
		else
		{
			uint ui32VertexBufferSize = vertexBuffer->getVertexSize();
			void*	pBufferData = 0;
			bool	bHasVBO = m_rsc.hasCapability(RSC_VBO);

			if(bHasVBO)
			{
				glBindBuffer(GL_ARRAY_BUFFER,static_cast<VertexBuffer*>(vertexBuffer)->getGLBufferId());
				pBufferData = VBO_BUFFER_OFFSET(start);
			}
			else
			{
				pBufferData = static_cast<DefaultVertexBuffer*>(vertexBuffer)->getDataPtr(start);
			}

			glColorPointer(4,GL_UNSIGNED_BYTE,ui32VertexBufferSize,pBufferData);
			glEnableClientState(GL_COLOR_ARRAY);
			if(bHasVBO)glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		*/
	}

	void		RenderSystem::setSpecularVertexBuffer(IVertexBuffer* vertexBuffer,uint start ,uint stride)
	{
		assert(0);
		/*
		if(!vertexBuffer)
		{
			glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
		}
		else
		{
			uint ui32VertexBufferSize = vertexBuffer->getVertexSize();
			void*	pBufferData = 0;
			bool	bHasVBO = m_rsc.hasCapability(RSC_VBO);

			if(bHasVBO)
			{
				glBindBuffer(GL_ARRAY_BUFFER,static_cast<VertexBuffer*>(vertexBuffer)->getGLBufferId());
				pBufferData = VBO_BUFFER_OFFSET(start);
			}
			else
			{
				pBufferData = static_cast<DefaultVertexBuffer*>(vertexBuffer)->getDataPtr(start);
			}

			glSecondaryColorPointerEXT(4,GL_UNSIGNED_BYTE,ui32VertexBufferSize,pBufferData);
			glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
			if(bHasVBO)glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		*/
	}

	void		RenderSystem::setBlendWeightVertexBuffer(IVertexBuffer* pVB, uint start)
	{
		//to be continued...
	}

	void		RenderSystem::setBlendIndicesVertexBuffer(IVertexBuffer* pVB, uint start)
	{
		//to be continued...
	}

	void		RenderSystem::setNormalVertexBuffer(IVertexBuffer* vertexBuffer,uint start,uint stride)
	{
		const AttrInfo* pInfo = getAttrInfo(EVA_NORMAL);
		GLint location = getAttrLocation(pInfo->usage,pInfo->index);
		if(!vertexBuffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER,0);
			glDisableVertexAttribArray(location);
			return;
		}

		VertexBuffer* pVb = static_cast<VertexBuffer*>(vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER,pVb->getGLBufferId());

		GLenum glType;
		GLint components;
		GLboolean normalized;
		getAttrGLInfo(pInfo->type,glType,components,normalized);

		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location
			, components
			, glType
			, normalized
			, stride
			, (const void*)(((char*)start))
			);
		TestGLError("setNormalVertexBuffer");

		/*
		if(!vertexBuffer)
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}
		else
		{
			uint ui32VertexBufferSize = vertexBuffer->getVertexSize();
			void*	pBufferData = 0;
			bool	bHasVBO = m_rsc.hasCapability(RSC_VBO);

			if(bHasVBO)
			{
				glBindBuffer(GL_ARRAY_BUFFER,static_cast<VertexBuffer*>(vertexBuffer)->getGLBufferId());
				pBufferData = VBO_BUFFER_OFFSET(start);
			}
			else
			{
				pBufferData = static_cast<DefaultVertexBuffer*>(vertexBuffer)->getDataPtr(start);
			}

			glNormalPointer(GL_FLOAT,ui32VertexBufferSize,pBufferData);
			glEnableClientState(GL_NORMAL_ARRAY);
			if(bHasVBO)glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		*/
	}

	void		RenderSystem::setIndexBuffer(IIndexBuffer* indexBuffer)
	{
		if(indexBuffer)
		{
			bool	bHasVBO = m_rsc.hasCapability(RSC_VBO);
			if(bHasVBO)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,static_cast<IndexBuffer*>(indexBuffer)->getGLBufferId());
			}
			else
			{
				m_pIndexBuffer = indexBuffer;
			}
		}
		else
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		}
	}

	void RenderSystem::drawRangeIndexedPrimitive(PrimitiveType pt,IndexType indexType,uint ui32IndexStart,uint ui32IndexCount,uint ui32VertexStart,uint ui32VertexEnd)
	{
		assert(0);//gles2 无类似接口，废除

		/*
		GLenum it = (indexType == IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
		uint ui32IndexSize = (indexType == IT_16BIT) ? 2 : 4;

		bool	bHasVBO = m_rsc.hasCapability(RSC_VBO);
		if(bHasVBO)
		{
			glDrawRangeElements(getPrimitiveType(pt),ui32VertexStart,ui32VertexEnd,ui32IndexCount,it,VBO_BUFFER_OFFSET(ui32IndexStart * ui32IndexSize));
		}
		else
		{
			if(m_pIndexBuffer)
			{
				glDrawRangeElements(getPrimitiveType(pt),ui32VertexStart,ui32VertexEnd,ui32IndexCount,it,(uchar*)static_cast<DefaultIndexBuffer*>(m_pIndexBuffer)->getDataPtr(0) + ui32IndexStart * ui32IndexSize);
			}
		}
		*/
	}

	void RenderSystem::drawIndexedPrimitive(PrimitiveType primitiveType,uint ui32IndexCount,IndexType indexType,void* pdata)
	{
		GLenum it = (indexType == IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
		glDrawElements(getPrimitiveType(primitiveType),ui32IndexCount,it,pdata);
	}

	void RenderSystem::drawPrimitive(PrimitiveType primitiveType,uint ui32VertexStart,uint ui32VertexCount)
	{
		glDrawArrays(getPrimitiveType(primitiveType),ui32VertexStart,ui32VertexCount);
	}

	void RenderSystem::setFog(FogMode mode,const ColorValue& color,float expDensity,float linearStart,float linearEnd)
	{
		assert(0);
		/*
		GLint fogMode;
		switch(mode)
		{
		case FOG_EXP:
			fogMode = GL_EXP;
			break;
		case FOG_EXP2:
			fogMode = GL_EXP2;
			break;
		case FOG_LINEAR:
			fogMode = GL_LINEAR;
			break;
		default:
			// Give up on it
			glDisable(GL_FOG);
			return;
		}

		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE,fogMode);
		GLfloat fogColor[4] = {color.r,color.g,color.b,color.a};
		glFogfv(GL_FOG_COLOR,fogColor);
		glFogf(GL_FOG_DENSITY,expDensity);
		glFogf(GL_FOG_START,linearStart);
		glFogf(GL_FOG_END,linearEnd);
		*/
	}

	void	RenderSystem::enableFog(bool enable)
	{
		//assert(0);
		//enable ? glEnable(GL_FOG) : glDisable(GL_FOG);
	}

	bool	RenderSystem::isFogEnabled()
	{
		//assert(0);
		return false;
		/*
		PP_BY_NAME("isFogEnabled");
		GLint fog;
		glGetIntegerv(GL_FOG,&fog);
		return fog != 0;
		*/
	}

	void		RenderSystem::setLight(ushort unit,Light* pLight)
	{
        //assert(0);
        printf("\n RenderSystem::setLight!!!  %d",unit);
		

		/*
		GLenum light = GL_LIGHT0 + unit;
		if (!pLight || !pLight->isEnabled())
		{
			glDisable(light);
		}
		else
		{
			LightType lt = pLight->getType();
			switch(lt)
			{
			case LT_SPOTLIGHT:
				glLightf(light,GL_SPOT_CUTOFF,0.5f * pLight->getSpotlightOuterAngle().valueDegrees());
				break;
			default:
				glLightf(light,GL_SPOT_CUTOFF,180.0);
				break;
			}

			// Color
			ColorValue color;
			color = pLight->getDiffuseColor();

			GLfloat f4vals[4] = {color.r,color.g,color.b,color.a};
			glLightfv(light,GL_DIFFUSE,f4vals);

			color = pLight->getSpecularColor();
			f4vals[0] = color.r;
			f4vals[1] = color.g;
			f4vals[2] = color.b;
			f4vals[3] = color.a;
			glLightfv(light,GL_SPECULAR,f4vals);

			color = pLight->getAmbientColor();
			f4vals[0] = color.r;
			f4vals[1] = color.g;
			f4vals[2] = color.b;
			f4vals[3] = color.a;
			glLightfv(light,GL_AMBIENT,f4vals);

			Vector3 v;
			float pos[4];
			if(lt == LT_DIRECTIONAL)
			{
				v = pLight->getDirection();
				pos[0] = v.x;
				pos[1] = v.y;
				pos[2] = v.z;
				pos[3] = 0;
			}
			else
			{
				v = pLight->getPosition();
				pos[0] = v.x;
				pos[1] = v.y;
				pos[2] = v.z;
				pos[3] = 1;
			}
			glLightfv(light,GL_POSITION,pos);

			if (lt == LT_SPOTLIGHT)
			{
				v = pLight->getDirection();
				pos[0] = v.x;
				pos[1] = v.y;
				pos[2] = v.z;
				pos[3] = 0;

				glLightfv(light,GL_SPOT_DIRECTION,pos);
			}

			// Attenuation
			glLightf(light,GL_CONSTANT_ATTENUATION,pLight->getAttenuationConstant());
			glLightf(light,GL_LINEAR_ATTENUATION,pLight->getAttenuationLinear());
			glLightf(light,GL_QUADRATIC_ATTENUATION,pLight->getAttenuationQuadric());
			// Enable in the scene
			glEnable(light);
		}
		*/
	}

	void		RenderSystem::setPointSize(float size)
	{
		assert(0);
		//glPointSize(size);
	}

	void RenderSystem::setTrackingColorEnum(TrackVertexColorEnum tracking)
	{
		assert(0);
		/*
		// Track vertex colour
		if(tracking != TVC_NONE) 
		{
			GLenum gt = GL_DIFFUSE;
			// There are actually 15 different combinations for tracking, of which
			// GL only supports the most used 5. This means that we have to do some
			// magic to find the best match. NOTE: 
			//  GL_AMBIENT_AND_DIFFUSE != GL_AMBIENT | GL__DIFFUSE
			if(tracking & TVC_AMBIENT_AND_DIFFUSE) 
			{
				gt = GL_AMBIENT_AND_DIFFUSE;
			}
			else if(tracking & TVC_AMBIENT)
			{
				gt = GL_AMBIENT;
			}
			else if(tracking & TVC_DIFFUSE) 
			{
				gt = GL_DIFFUSE;
			}
			else if(tracking & TVC_SPECULAR) 
			{
				gt = GL_SPECULAR;              
			}
			else if(tracking & TVC_EMISSIVE) 
			{
				gt = GL_EMISSION;
			}
			glColorMaterial(GL_FRONT_AND_BACK, gt);

			glEnable(GL_COLOR_MATERIAL);
		} 
		else 
		{
			glDisable(GL_COLOR_MATERIAL);          
		}
		*/
	}

	const ColorValue&	RenderSystem::getSurfaceDiffuse()
	{
		return m_surfaceDiffuse;
	}

	void RenderSystem::setSurfaceDiffuse(const ColorValue &diffuse)
	{
		m_surfaceDiffuse = diffuse;
		//GLfloat f4val[4] = {diffuse.r,diffuse.g,diffuse.b,diffuse.a};
		//glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,f4val);

		/*
		if(m_pCurrentShaderProgram)
		{
			Vector4 c(diffuse.r,diffuse.g,diffuse.b,diffuse.a);
			m_pCurrentShaderProgram->setUniformVector4(G_DIFFUSE,c);
		}
		*/
		IHighLevelShaderProgram* pShader = static_cast<IHighLevelShaderProgram*>(getShaderProgramManager()->getCurrentShaderProgam());
		if(pShader)
		{
			//Vector4 c(diffuse.r,diffuse.g,diffuse.b,diffuse.a);
			//pShader->setUniformVector4(G_DIFFUSE,c);
			pShader->bindTransformMatrix(TMT_DIFFUSE_COLOR);
		}
	}

	//-----------------------------------------------------------------------------
	void RenderSystem::setSurfaceParams(
		const ColorValue &ambient,
		const ColorValue &diffuse,
		const ColorValue &specular,
		const ColorValue &emissive,
		float shininess)
	{
		m_surfaceDiffuse = diffuse;
		//assert(0);
		printf("\n RenderSystem::setSurfaceParams");
        
        /*
		GLfloat f4val[4] = {diffuse.r,diffuse.g,diffuse.b,diffuse.a};
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,f4val);
		f4val[0] = ambient.r;
		f4val[1] = ambient.g;
		f4val[2] = ambient.b;
		f4val[3] = ambient.a;
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,f4val);
		f4val[0] = specular.r;
		f4val[1] = specular.g;
		f4val[2] = specular.b;
		f4val[3] = specular.a;
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,f4val);
		f4val[0] = emissive.r;
		f4val[1] = emissive.g;
		f4val[2] = emissive.b;
		f4val[3] = emissive.a;
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,f4val);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
		*/
	}
	void RenderSystem::setClipPlane(uint index,const Plane*pPlane)
	{
		assert(0);

		/*
		GLenum ePlane = static_cast<GLenum>(GL_CLIP_PLANE0 + index);
		if(!pPlane)
		{
			glDisable(ePlane);
			return;
		}

		GLint maxPlanes;
		glGetIntegerv(GL_MAX_CLIP_PLANES,&maxPlanes);
		if(index >= (uint)maxPlanes)
		{
			Trace("setClipPlane,index >= maxPlanes\n");
			return;
		}

		GLfloat clipPlane[4];
		clipPlane[0] = pPlane->normal.x;
        clipPlane[1] = pPlane->normal.y;
		clipPlane[2] = pPlane->normal.z;
		clipPlane[3] = pPlane->d;

		glClipPlane(ePlane,clipPlane);
		glEnable(ePlane);
		*/
	}

	extern "C"{extern void glutSolidTeapot(GLfloat scale);}
	/*
	.SH DESCRIPTION
	glutSolidTeapot and glutWireTeapot render a solid or wireframe
	teapot respectively. Both surface normals and texture coordinates for the
	teapot are generated. The teapot is generated with OpenGL evaluators. 
	.SH BUGS
	The teapot is greatly over-tesselated; it renders way too slow.

	OpenGL's default glFrontFace state assumes that front facing polygons
	(for the purpose of face culling)
	have vertices that wind counter clockwise when projected into window
	space.  This teapot is rendered with its front facing polygon vertices
	winding clockwise.  For OpenGL's default back face culling to work,
	you should use:
	.nf
	.LP
	glFrontFace(GL_CW);
	glutSolidTeapot(size);
	glFrontFace(GL_CCW);
	.fi
	.LP
	Both these bugs reflect issues in the original aux toolkit's
	teapot rendering routines (GLUT used the same teapot rendering routine).
	.SH SEE ALSO
	glutSolidSphere, glutSolidCube, glutSolidCone, glutSolidTorus, glutSolidDodecahedron,
	glutSolidOctahedron, glutSolidTetrahedron, glutSolidIcosahedron
	.SH AUTHOR
	Mark J. Kilgard (mjk@nvidia.com)
	 */
	void RenderSystem::teapot(float size,const ColorValue& color)
	{
		/*
		ColorValue c = m_pCurrentRenderTarget->m_RenderState.m_color;
		CullingMode cm = m_pCurrentRenderTarget->m_RenderState.m_cullingMode;
		setColor(color);
		setCullingMode(CULL_COUNTERCLOCKWISE);
		glutSolidTeapot(size);
		setCullingMode(cm);
		setColor(c);
		*/
	}

	void RenderSystem::line(const Point& ptFirst,const Point& ptSecond,const ColorValue& color)
	{
		ColorValue c = m_pCurrentRenderTarget->m_RenderState.m_color;
		setColor(color);

		beginPrimitive(PT_LINES);
		_setTempTexture();
		sendVertex(Vector2(ptFirst.x,ptFirst.y));
		sendVertex(Vector2(ptSecond.x,ptSecond.y));
		endPrimitive();

		setColor(c);
	}

	void RenderSystem::rectangle(const Rect& rc,const ColorValue& color)
	{
		

		ColorValue c = m_pCurrentRenderTarget->m_RenderState.m_color;
		setColor(color);

		beginPrimitive(PT_LINES);
		_setTempTexture();
		sendVertex(Vector2(rc.left,rc.top));
		sendVertex(Vector2(rc.left,rc.bottom));
		sendVertex(Vector2(rc.left,rc.bottom));
		sendVertex(Vector2(rc.right,rc.bottom));
		sendVertex(Vector2(rc.right,rc.bottom));
		sendVertex(Vector2(rc.right,rc.top));
		sendVertex(Vector2(rc.right,rc.top));
		sendVertex(Vector2(rc.left,rc.top));
		endPrimitive();

		setColor(c);
	}

	void RenderSystem::rectangle(const Rect& rc,const ColorValue& cLeftTop,const ColorValue& cLeftBottom,const ColorValue& cRightTop,const ColorValue& cRightBottom)
	{
		
		ColorValue c = m_pCurrentRenderTarget->m_RenderState.m_color;

		beginPrimitive(PT_LINES);
		_setTempTexture();
		setColor(cLeftTop);
		sendVertex(Vector2(rc.left,rc.top));
		sendVertex(Vector2(rc.left,rc.bottom));
		setColor(cLeftBottom);
		sendVertex(Vector2(rc.left,rc.bottom));
		sendVertex(Vector2(rc.right,rc.bottom));
		setColor(cRightBottom);
		sendVertex(Vector2(rc.right,rc.bottom));
		sendVertex(Vector2(rc.right,rc.top));
		setColor(cRightTop);
		sendVertex(Vector2(rc.right,rc.top));
		sendVertex(Vector2(rc.left,rc.top));
		endPrimitive();

		setColor(c);
	}

	void RenderSystem::point(const Point& pt,const ColorValue& color)
	{
		ColorValue c = m_pCurrentRenderTarget->m_RenderState.m_color;
		setColor(color);

		beginPrimitive(PT_POINTS);
		_setTempTexture();
		sendVertex(Vector2(pt.x,pt.y));
		endPrimitive();

		setColor(c);
	}

	void RenderSystem::rectangle(const Rect& rc,ITexture* pTexture, const Vector2 & vLetfTop, const Vector2 & vLeftBottom, const Vector2 & vRightBottom, const Vector2 & RightTop)
	{
		beginPrimitive(PT_QUADS);

		if(pTexture)
			setTexture(0,pTexture);
		else
			_setTempTexture();

		setTexcoord(vLetfTop);
		sendVertex(Vector2(rc.left,rc.top));
		setTexcoord(vLeftBottom);
		sendVertex(Vector2(rc.left,rc.bottom));
		setTexcoord(vRightBottom);
		sendVertex(Vector2(rc.right,rc.bottom));
		setTexcoord(RightTop);
		sendVertex(Vector2(rc.right,rc.top));
		endPrimitive();

		setTexture(0,0);
	}

	void		RenderSystem::rectangle(const Rect& rc,ITexture* pTexture)
	{
		beginPrimitive(PT_QUADS);

		if(pTexture)
			setTexture(0,pTexture);
		else
			_setTempTexture();

		setTexcoord(Vector2(0,0));
		sendVertex(Vector2(rc.left,rc.top));
		setTexcoord(Vector2(0,1));
		sendVertex(Vector2(rc.left,rc.bottom));
		setTexcoord(Vector2(1,1));
		sendVertex(Vector2(rc.right,rc.bottom));
		setTexcoord(Vector2(1,0));
		sendVertex(Vector2(rc.right,rc.top));
		endPrimitive();

		setTexture(0,0);
	}

	void		RenderSystem::box(const Rect& r,const ColorValue& color)
	{
		ColorValue c = m_pCurrentRenderTarget->m_RenderState.m_color;
		setColor(color);

		Rect rc(r.left,r.top - 1,r.right + 1,r.bottom);
		beginPrimitive(PT_QUADS);
		_setTempTexture();
		sendVertex(Vector2(rc.left,rc.top));
		sendVertex(Vector2(rc.left,rc.bottom));
		sendVertex(Vector2(rc.right,rc.bottom));
		sendVertex(Vector2(rc.right,rc.top));
		endPrimitive();

		setColor(c);
	}


	Vector3		RenderSystem::unproject(const Vector3& v)
	{
		GLfloat mm[16];
		GLfloat pm[16];

		Matrix4 mtxProj = m_pCurrentRenderTarget->m_RenderState.m_mtxProjection;
		mtxProj = mtxProj.transpose();
		for(int i = 0;i < 16;i++)pm[i] = mtxProj[i >> 2][i % 4];

		Matrix4 mtxModelView = m_pCurrentRenderTarget->m_RenderState.m_mtxModelView;
		mtxModelView = mtxModelView.transpose();
		for(int i = 0;i < 16;i++)mm[i] = mtxModelView[i >> 2][i % 4];

		/*glGetDoublev(GL_MODELVIEW_MATRIX,mm);
		glGetDoublev(GL_PROJECTION_MATRIX,pm);*/
		//for(int i = 0;i < 16;i++)
		//{
		//	mm[i] = m_mtxModelView[i - ((i >> 2) << 2)][i >> 2];
		//	pm[i] = m_mtxProjection[i - ((i >> 2) << 2)][i >> 2];
		//}

		xs::Rect rc;
		m_pCurrentRenderTarget->getRect(&rc);

		GLint vp[4];
		vp[0] = m_pCurrentRenderTarget->m_vpLeft;
		vp[1] = m_pCurrentRenderTarget->m_vpTop;
		vp[2] = m_pCurrentRenderTarget->m_vpWidth;
		vp[3] = m_pCurrentRenderTarget->m_vpHeight;

		assert(0);
		GLfloat x,y,z;
		//gluUnProject(v.x,rc.bottom - rc.top - v.y,v.z,mm,pm,vp,&x,&y,&z);

		return Vector3(x,y,z);
	}

	Vector3		RenderSystem::project(const Vector3& v)
	{
		GLfloat mm[16];
		GLfloat pm[16];

		Matrix4 mtxProj = m_pCurrentRenderTarget->m_RenderState.m_mtxProjection;
		mtxProj = mtxProj.transpose();
		for(int i = 0;i < 16;i++)pm[i] = mtxProj[i >> 2][i % 4];

		Matrix4 mtxModelView = m_pCurrentRenderTarget->m_RenderState.m_mtxModelView;
		mtxModelView = mtxModelView.transpose();
		for(int i = 0;i < 16;i++)mm[i] = mtxModelView[i >> 2][i % 4];
		
		/*PROFILE_BY_NAME_START("GL_MODELVIEW_MATRIX");
		glGetDoublev(GL_MODELVIEW_MATRIX,mm);
		PROFILE_BY_NAME_STOP();
		PROFILE_BY_NAME_START("GL_PROJECTION_MATRIX");
		glGetDoublev(GL_PROJECTION_MATRIX,pm);
		PROFILE_BY_NAME_STOP();*/
		//for(int i = 0;i < 16;i++)
		//{
		//	mm[i] = m_mtxModelView[i - ((i >> 2) << 2)][i >> 2];
		//	pm[i] = m_mtxProjection[i - ((i >> 2) << 2)][i >> 2];
		//}

		GLint vp[4];
		vp[0] = m_pCurrentRenderTarget->m_vpLeft;
		vp[1] = m_pCurrentRenderTarget->m_vpTop;
		vp[2] = m_pCurrentRenderTarget->m_vpWidth;
		vp[3] = m_pCurrentRenderTarget->m_vpHeight;

		xs::Rect rc;
		m_pCurrentRenderTarget->getRect(&rc);

		assert(0);
		GLfloat x,y,z;
		//gluProject(v.x,v.y,v.z,mm,pm,vp,&x,&y,&z);

		return Vector3(x,rc.bottom - rc.top - y,z);
	}

	void 		RenderSystem::setVerticalSync(bool vsync)
	{
#ifdef _WIN32
		//if(__wglewSwapIntervalEXT)__wglewSwapIntervalEXT(vsync);
#endif
	}

	//---------------------------------------------------------------------
	void  RenderSystem::setStencilCheckEnabled(bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_STENCIL_TEST);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}
	}

	//---------------------------------------------------------------------
	GLint convertStencilOp(StencilOperation op)
	{
		switch(op)
		{
		case SOP_KEEP:
			return GL_KEEP;
		case SOP_ZERO:
			return GL_ZERO;
		case SOP_REPLACE:
			return GL_REPLACE;
		case SOP_INCREMENT:
			return GL_INCR;
		case SOP_DECREMENT:
			return GL_DECR;
		case SOP_INCREMENT_WRAP:
			//return GL_INCR_WRAP_EXT;
			assert(0);
			return SOP_KEEP;
		case SOP_DECREMENT_WRAP:
			//return GL_DECR_WRAP_EXT;
			assert(0);
			return SOP_KEEP;
		case SOP_INVERT:
			return GL_INVERT;
		};
		// to keep compiler happy
		return SOP_KEEP;
	}

	//---------------------------------------------------------------------
	void  RenderSystem::setStencilBufferParams(
		CompareFunction func, 
		uint refValue,
		uint mask,
		StencilOperation stencilFailOp,
		StencilOperation depthFailOp,
		StencilOperation passOp)
	{
		glStencilMask(mask);
		glStencilFunc(convertCompareFunction(func), refValue, mask);
		glStencilOp(convertStencilOp(stencilFailOp),convertStencilOp(depthFailOp),convertStencilOp(passOp));
	}

	void RenderSystem::setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
	{
		glColorMask(red, green, blue, alpha);
	}

	void RenderSystem::beginSelection(int x,int y,uint variance)
	{
		if(m_RenderMode == RENDER_MODE_SELECTION)return;
		m_RenderMode = RENDER_MODE_SELECTION;

		assert(0);
		
#if 0
		GLint view[4];

		/*
		This choose the buffer where store the values for the selection data
		*/
		glSelectBuffer(64, m_SelectionBuffer);

		/*
		This retrieve info about the viewport
		*/
		glGetIntegerv(GL_VIEWPORT, view);

		/*
		Switching in selecton mode
		*/
		glRenderMode(GL_SELECT);

		/*
		Clearing the name's stack
		This stack contains all the info about the objects
		*/
		glInitNames();

		/*
		Now fill the stack with one element (or glLoadName will generate an error)
		*/
		glPushName(0);

		/*
		Now modify the vieving volume, restricting selection area around the cursor
		*/
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		/*
		restrict the draw to an area around the cursor
		*/
		gluPickMatrix(x - variance, (view[3] - view[1]) - y - variance, 1 + 2 * variance, 1 + 2 * variance, view);

		GLfloat pm[16];
		glGetDoublev(GL_PROJECTION_MATRIX,pm);
		Matrix4 mtx(
			pm[0],pm[4],pm[8],pm[12],
			pm[1],pm[5],pm[9],pm[13],
			pm[2],pm[6],pm[10],pm[14],
			pm[3],pm[7],pm[11],pm[15]);
		mtx = mtx * m_pCurrentRenderTarget->m_RenderState.m_mtxProjection;
		Matrix4 m = mtx.transpose();
		glLoadMatrixf(&m[0][0]);

		/*
		Draw the objects onto the screen
		*/
		glMatrixMode(GL_MODELVIEW);
#endif
	}

	void RenderSystem::endSelection(int& num,int* pBuffer)
	{
		if(m_RenderMode != RENDER_MODE_SELECTION)return;
		m_RenderMode = RENDER_MODE_NORMAL;

		assert(0);

#if 0
		GLint hits;
		/*
		Do you remeber? We do pushMatrix in PROJECTION mode
		*/
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glFlush();
		/*
		get number of objects drawed in that area
		and return to render mode
		*/
		hits = glRenderMode(GL_RENDER);

		glMatrixMode(GL_MODELVIEW);

		if(num <= 0)return;
		int n = (num > hits ? hits : num);
		num = n;
		for(int i = 0;i < n;i++)
		{
			pBuffer[i] = (GLubyte)m_SelectionBuffer[i * 4 + 3];
		}
#endif
	}

	void RenderSystem::setSelectionIndex(int index)
	{
		//glLoadName(index);
		assert(0);
	}

	const RenderMode&	RenderSystem::getRenderMode()
	{
		return m_RenderMode;
	}

	RenderSystemType RenderSystem::getRenderSystemType()
	{
		return RS_OPENGLES2;
	}

	bool RenderSystem::setTextureStageStatus(	
		uint stage,
		TextureStageOperator colorOP,
		TextureStageArgument colorArg0,
		TextureStageArgument colorArg1,
		TextureStageOperator alphaOP,
		TextureStageArgument alphaArg0,
		TextureStageArgument alphaArg1)
	{
		//为什么要在这里return呢？
		//代码经过测试和预想的一样。但是用在客户端绘制小地图时，却出现了小地图全白。晕！
		//多线程的原因，。。。。，让人纠结，所以直接return了。
		return false;
		
		/*
		GLint textureLimit0;
		glGetIntegerv(GL_MAX_TEXTURE_COORDS,&textureLimit0);
		GLint textureLimit1;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureLimit1);
		if( stage >= (uint)textureLimit0 || stage >=  (uint)textureLimit1)
			return false;

		GLint activeTexture;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
		glActiveTexture( stage );

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

		GLint value;
		
		value = getTextureStageOp(colorOP);
		glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, value);
		value = getTextureStageArg(colorArg0);
		glTexEnvi( GL_TEXTURE_ENV, GL_SRC0_RGB, value);
		value = getTextureStageArg( colorArg1);
		glTexEnvi( GL_TEXTURE_ENV, GL_SRC1_RGB, value);

		value = getTextureStageOp( alphaOP);
		glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, value);
		value = getTextureStageArg( alphaArg0);
		glTexEnvi( GL_TEXTURE_ENV, GL_SRC0_ALPHA, value);
		value = getTextureStageArg( alphaArg1);
		glTexEnvi( GL_TEXTURE_ENV, GL_SRC1_ALPHA, value);

		glActiveTexture(activeTexture);
		return true;
		*/
	}

	void RenderSystem::getTextureStageStatus(	
		uint stage,
		TextureStageOperator & colorOP,
		TextureStageArgument & colorArg0,
		TextureStageArgument & colorArg1,
		TextureStageOperator & alphaOP,
		TextureStageArgument & alphaArg0,
		TextureStageArgument & alphaArg1)
	{
		assert(0);
		/*
		GLint textureLimit0;
		glGetIntegerv(GL_MAX_TEXTURE_COORDS,&textureLimit0);
		GLint textureLimit1;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureLimit1);
		if( stage >= (uint)textureLimit0 || stage >=  (uint)textureLimit1)
			return;

		GLint activeTexture;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
		glActiveTexture( stage );

		GLint value;
		glGetTexEnviv(GL_TEXTURE_ENV, GL_COMBINE_RGB, &value);
		colorOP = getTextureStageOp( value);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_SRC0_RGB, &value);
		colorArg0 = getTextureStageArg(value);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_SRC1_RGB, &value);
		colorArg1 = getTextureStageArg(value);

		glGetTexEnviv(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, &value);
		alphaOP = getTextureStageOp( value);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_SRC0_ALPHA, &value);
		alphaArg0 = getTextureStageArg(value);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_SRC1_ALPHA, &value);
		alphaArg1 = getTextureStageArg(value);

		glActiveTexture(activeTexture);
		return;
		*/
	}

	TextureStageOperator RenderSystem::getTextureStageOp(GLint op)
	{
		switch( op)
		{
		/*
		case GL_REPLACE:
			return TSO_REPLACE;
		case GL_MODULATE:
			return TSO_MODULATE;
		case GL_ADD:
			return TSO_ADD;
		case GL_ADD_SIGNED:
			return TSO_ADD_SIGNED;
		case GL_SUBTRACT:
			return TSO_SUBTRACT;
		*/
		default://绝不会到这里
			assert(0);
			return TSO_MODULATE;
		}
	}

	GLint RenderSystem::getTextureStageOp( TextureStageOperator op)
	{
		assert(0);
		return 0;
		/*
		switch( op)
		{
			
		case TSO_REPLACE:
			return GL_REPLACE;
		case TSO_MODULATE:
			return GL_MODULATE;
		case TSO_ADD:
			return GL_ADD;
		case TSO_ADD_SIGNED:
			return GL_ADD_SIGNED;
		case TSO_SUBTRACT:
			return GL_SUBTRACT;
			
		default://绝不会到这里
			
			return GL_MODULATE;
		}
		*/
	}

	TextureStageArgument RenderSystem::getTextureStageArg(GLint arg)
	{
		switch( arg)
		{
		case GL_TEXTURE:
			return TSA_TEXTURE;
		//case GL_PREVIOUS:
		//	return TSA_PREVIOUS;
		default://never goes to here
			assert(0);
			return TSA_TEXTURE;
		}
	
	}

	GLint RenderSystem::getTextureStageArg(TextureStageArgument arg)
	{
		switch( arg)
		{
		case TSA_TEXTURE:
			return GL_TEXTURE;
		//case TSA_PREVIOUS:
		//	return GL_PREVIOUS;
		default://never goes to here
			assert(0);
			return GL_TEXTURE;
		}
	}

	IHardwareCursorManager * RenderSystem::getHardwareCursorManager()
	{
		return HardwareCursorManagerOGL::instance();
	}

	void RenderSystem::_checkCacheAndFlush(unsigned int count)
	{
		if(m_batchStatus.isOverCache(count))
			_flush();
	}

	void RenderSystem::_flush()
	{
		if(m_batchStatus.vertexCount > 0)
		{
			int copyColorNum = m_batchStatus.vertexCount - m_batchStatus.colorCount;
			if(copyColorNum > 0)
			{
				unsigned int c =  m_pCurrentRenderTarget->m_RenderState.m_color.getAsABGR();//getAsRGBA()	;
				for(int i = 0;i < copyColorNum;++i)
				{
					m_batchStatus.colors[i + m_batchStatus.colorCount] = c;
				}
			}
			{
				const AttrInfo* pInfo = getAttrInfo(EVA_COLOR);
				GLint location = getAttrLocation(pInfo->usage,pInfo->index);

				GLenum glType;
				GLint components;
				GLboolean normalized;
				getAttrGLInfo(pInfo->type,glType,components,normalized);

				glEnableVertexAttribArray(location);
				glVertexAttribPointer(location
					, components
					, glType
					, normalized
					, 0
					, (const void*)(m_batchStatus.colors)
					);
			}


			if(m_batchStatus.flagBits.vertex)
			{
				const AttrInfo* pInfo =  getAttrInfo(EVA_POSITION);
				GLint location = getAttrLocation(pInfo->usage,pInfo->index);

				GLenum glType;
				GLint components;
				GLboolean normalized;
				getAttrGLInfo(pInfo->type,glType,components,normalized);

				glEnableVertexAttribArray(location);
				glVertexAttribPointer(location
					, components
					, glType
					, normalized
					, 0
					, (const void*)(m_batchStatus.vertexs)
					);
			}

			if(m_batchStatus.flagBits.normal)
			{
				assert(0);

				const AttrInfo* pInfo =  getAttrInfo(EVA_NORMAL);
				GLint location = getAttrLocation(pInfo->usage,pInfo->index);

				GLenum glType;
				GLint components;
				GLboolean normalized;
				getAttrGLInfo(pInfo->type,glType,components,normalized);

				glEnableVertexAttribArray(location);
				glVertexAttribPointer(location
					, components
					, glType
					, normalized
					, 0
					, (const void*)(m_batchStatus.normals)
					);
			}

			if(m_batchStatus.flagBits.texCoord0)
			{
				const AttrInfo* pInfo =  getAttrInfo(EVA_TEX_COORDS_0);
				GLint location = getAttrLocation(pInfo->usage,pInfo->index);

				GLenum glType;
				GLint components;
				GLboolean normalized;
				getAttrGLInfo(pInfo->type,glType,components,normalized);

				glEnableVertexAttribArray(location);
				glVertexAttribPointer(location
					, components
					, glType
					, normalized
					, 0
					, (const void*)(m_batchStatus.texCoord0s)
					);
			}

			if(m_batchStatus.flagBits.texCoord1)
			{
				assert(0);

				const AttrInfo* pInfo =  getAttrInfo(EVA_TEX_COORDS_1);
				GLint location = getAttrLocation(pInfo->usage,pInfo->index);

				GLenum glType;
				GLint components;
				GLboolean normalized;
				getAttrGLInfo(pInfo->type,glType,components,normalized);

				glEnableVertexAttribArray(location);
				glVertexAttribPointer(location
					, components
					, glType
					, normalized
					, 0
					, (const void*)(m_batchStatus.texCoord1s)
					);
			}

			//set world_view_proj to shader!
			_setWorldViewProj2Shader();

			if(PT_QUADS == m_batchStatus.pt)
			{
				unsigned int indexunm = (m_batchStatus.vertexCount>>2)*6;
				drawIndexedPrimitive(PT_TRIANGLES,indexunm,IT_16BIT,m_batchStatus.indexs);
			}
			else
			{
				drawPrimitive(m_batchStatus.pt,0,m_batchStatus.vertexCount);
			}
		}
		m_batchStatus.clear();
	}


	bool RenderSystem::_createInnerShader()
	{
		IShaderProgramManager* pShaderProgamMgr = getShaderProgramManager();
		
		std::string vs;
		std::string fs;
		for(int i = 0;i < ESP_NUM;++i)
		{
			m_pInnerShader[i] = static_cast<IHighLevelShaderProgram*>(pShaderProgamMgr->createShaderProgram(SPT_HIGHLEVEL));
			vs = "data/Shader/OGLES2/";
			fs = "data/Shader/OGLES2/";
			
			vs += g_shader_program_name[i];
			vs += VS;
			
			fs += g_shader_program_name[i];
			fs += FS;
			
			m_pInnerShader[i]->addShaderFromFile(ST_VERTEX_PROGRAM,vs);
			m_pInnerShader[i]->addShaderFromFile(ST_FRAGMENT_PROGRAM,fs);
			m_pInnerShader[i]->link();
		}
		return true;
	}

	void RenderSystem::_releaseInnerShader()
	{
		for(int i = 0;i < 4;i++)
		{
			if(m_pInnerShader[i])
			{
				safeRelease(m_pInnerShader[i]);
			}
		}
	}

	IHighLevelShaderProgram* RenderSystem::getShaderProgram(int id)
	{
		if(id >= ESP_NUM)
			return 0;

		return m_pInnerShader[id];
	}

	/*
	void RenderSystem::bindCurrentShaderProgram(IShaderProgram* pShaderProgram,bool setShaderConst)
	{
		if(m_pCurrentShaderProgram == pShaderProgram)
			return;

		if(pShaderProgram && pShaderProgram->getProgramType() != SPT_HIGHLEVEL)
			return;

		if(m_pCurrentShaderProgram)
		{
			m_pCurrentShaderProgram->unbind();
		}
		m_pCurrentShaderProgram = static_cast<IHighLevelShaderProgram*>(pShaderProgram);
		if(m_pCurrentShaderProgram)
		{
			m_pCurrentShaderProgram->bind();

			if(setShaderConst)
			{
				_setWorldViewProj2Shader();
			}
		}
	}
	*/

	void RenderSystem::_setWorldViewProj2Shader()
	{
		/*
		if(m_pCurrentShaderProgram)
		{
			Matrix4 proj = getProjectionMatrix() * getModelViewMatrix();
			//Matrix4 m = proj.transpose(); //是否需要。。。要仔细和gl版本比对
			m_pCurrentShaderProgram->setUniformMatrix(G_WORLD_VIEW_PROJ,proj,false);
		}
		*/

		IHighLevelShaderProgram* pShader = static_cast<IHighLevelShaderProgram*>(getShaderProgramManager()->getCurrentShaderProgam());
		if(pShader)
		{
			pShader->bindTransformMatrix(TMT_WORLD_VIEW_PROJECTION);
		}
	}

///----------------------------------------------------------------
	RenderSystem::BatchStatus::BatchStatus()
	{
		pt = PT_POINTS;
		isPrimitiveing = false;

		vertexs = new Vector3[VERTEX_CACHE_MAX];
		normals = new Vector3[VERTEX_CACHE_MAX];
		texCoord0s = new Vector2[VERTEX_CACHE_MAX];
		texCoord1s = new Vector2[VERTEX_CACHE_MAX];
		colors = new unsigned int[VERTEX_CACHE_MAX];

		unsigned int indexnum = VERTEX_CACHE_MAX/4*6;
		indexs = new unsigned short[indexnum];
		unsigned int vb = 0;
		for(unsigned int i = 0;i < indexnum; i+=6 ,vb+=4)
		{
			indexs[i] = vb;
			indexs[i+1] = vb+1;
			indexs[i+2] = vb+3;

			indexs[i+3] = vb+1;
			indexs[i+4] = vb+2;
			indexs[i+5] = vb+3;
		}
		clear();
	}

	RenderSystem::BatchStatus::~BatchStatus()
	{
		if(vertexs) delete[] vertexs;
		if(normals) delete[] normals;
		if(texCoord0s) delete[] texCoord0s;
		if(texCoord1s) delete[] texCoord1s;
		if(colors) delete[] colors;
		if(indexs) delete[] indexs;
	}

	bool RenderSystem::BatchStatus::isOverCache(unsigned int count)
	{
		if(count >= VERTEX_CACHE_MAX)
			return true;
		return false;
	}

	void RenderSystem::BatchStatus::clear()
	{
		vertexflag = 0;
		vertexCount = 0;
		normalCount = 0;
		texCoord0Count = 0;
		texCoord1Count = 0;
		colorCount = 0;
	}
}