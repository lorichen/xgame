#include "StdAfx.h"

#ifdef GLEW_MX
	GLEWContext _glewctx;
	#ifdef _WIN32
		WGLEWContext _wglewctx;
	#elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX)
		GLXEWContext _glxewctx;
	#endif
#endif