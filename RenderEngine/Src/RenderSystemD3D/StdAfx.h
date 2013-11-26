#ifndef __Pch_H__
#define __Pch_H__

#if defined(__WIN32__) || defined(_WIN32)
#include "windows.h"
#else
#error Need Win32 Platform 
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

#include <d3d9.h>
#include <d3dx9.h>
#define COM_SAFE_RELEASE(ptr) if( (ptr) ) { (ptr)->Release(); (ptr)=0;}

#if _MSC_VER >= 1200
#define RKT_FORCEINLINE __forceinline
#else
#define RKT_FORCEINLINE inline
#endif


#endif

