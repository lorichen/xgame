#ifndef __Pch_H__
#define __Pch_H__

#if defined(__WIN32__) || defined(_WIN32)
#include "windows.h"
#endif

#include "Base.h"
#define new RKT_NEW
#include "ReMath.h"
#include "RenderEngine/RenderEngineInc.h"
#include "RenderEngine/RenderEngineCommon.h"
#include "RenderEngine/IFontManager.h"
#include "RenderEngine/BufferDef.h"
#include "RenderEngine/IBufferManager.h"
#include "RenderEngine/VertexData.h"
#include "RenderEngine/IndexData.h"
#include "RenderEngine/PixelFormat.h"
#include "RenderEngine/Image.h"
#include "RenderEngine/ITextureManager.h"
#include "RenderEngine/RenderOperation.h"
#include "RenderEngine/IRenderEngine.h"
#include "RenderEngine/RenderSystemCapabilities.h"
#include "RenderEngine/IRenderSystem.h"
#include "RenderEngine/Light.h"
#include "RenderEngine/IShaderManager.h"
#include "RenderEngine/Viewport.h"

using namespace xs;

#if (TARGET_PLATFORM == PLATFORM_WIN32)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#else
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif



#define GL_WRITE_ONLY   GL_WRITE_ONLY_OES
#define GL_READ_ONLY	GL_BUFFER_ACCESS_OES
#define GL_READ_WRITE	GL_BUFFER_MAPPED_OES

inline bool TestGLError(const char* pszLocation)
{
	GLint iErr = glGetError();
	if (iErr != GL_NO_ERROR)
	{
		printf("%s failed (%d).\n", pszLocation, iErr);
		return false;
	}
	return true;
}	

#include "IPerfProf.h"

#endif