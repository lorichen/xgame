#ifndef __Pch_H__
#define __Pch_H__



//#include "PngOutlineText.h"
//using namespace TextDesigner;

#include "Base.h"

#if (TARGET_PLATFORM == PLATFORM_WIN32)
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;
#else

#endif


#define new RKT_NEW
#include "ReMath.h"
#include "RenderEngineInc.h"
#include "RenderEngine/RenderEngineCommon.h"
#include "RenderEngine/IRenderEngine.h"
#include "RenderEngine/Camera.h"
#include "RenderEngine/Viewport.h"
#include "RenderEngine/BufferDef.h"
#include "RenderEngine/IBufferManager.h"
#include "RenderEngine/IFontManager.h"
#include "RenderEngine/Light.h"
#include "RenderEngine/PixelFormat.h"
#include "RenderEngine/VertexData.h"
#include "RenderEngine/ITextureManager.h"
#include "RenderEngine/RenderOperation.h"
#include "RenderEngine/IRenderSystem.h"
#include "RenderEngine/RenderSystemCapabilities.h"
#include "RenderEngine/KeyFrames.h"
#include "RenderEngine/Material.h"
#include "RenderEngine/ModelData.h"
#include "RenderEngine/IModelManager.h"
#include "RenderEngine/IRenderable.h"
#include "RenderEngine/IShaderManager.h"
#include "RenderEngine/SharedPtr.h"
#include "RenderEngine/RenderQueue.h"
#include "RenderEngine/ImageRect.h"
#include "RenderEngine/shaderdeclare.h"

#include "IPerfProf.h"



#endif