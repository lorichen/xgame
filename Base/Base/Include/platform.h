
#ifndef __PLATFORM__H__
#define __PLATFORM__H__


//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro
#define PLATFORM_UNKNOWN			0
#define PLATFORM_IOS				1
#define PLATFORM_ANDROID			2
#define PLATFORM_WIN32				3
#define PLATFORM_LINUX				4
#define PLATFORM_MAC				5

// define compiler macro
#define COMPILER_NONE				0
#define COMPILER_MSVC 				1
#define COMPILER_GNUC 				2
#define COMPILER_BORL 				3
#define COMPILER_WINSCW 			4
#define COMPILER_GCCE 				5

// define architecture macro
#define ARCHITECTURE_32 			1
#define ARCHITECTURE_64 			2


//////////////////////////////////////////////////////////////////////////
// Finds the compiler type and version.
//////////////////////////////////////////////////////////////////////////
#define COMPILER			 		COMPILER_NONE

#if defined( __GCCE__ )
#	undef COMPILER
#	define COMPILER					COMPILER_GCCE
#	define COMP_VER					_MSC_VER
//#	pragma message("COMPILER = COMPILER_GCCE")
//#	include <staticlibinit_gcce.h> // This is a GCCE toolchain workaround needed when compiling with GCCE
#elif defined( __WINSCW__ )
#	undef COMPILER
#	define COMPILER					COMPILER_WINSCW
#	define COMP_VER					_MSC_VER
//#	pragma message("COMPILER = COMPILER_WINSCW")
#elif defined( _MSC_VER )
#	undef COMPILER
#	define COMPILER					COMPILER_MSVC
#	define COMP_VER					_MSC_VER
//#	pragma message("COMPILER = COMPILER_MSVC")
#elif defined( __GNUC__ )
#	undef COMPILER
#	define COMPILER					COMPILER_GNUC
#	define COMP_VER					(((__GNUC__)*100) + (__GNUC_MINOR__*10) + __GNUC_PATCHLEVEL__)
//#	pragma message("COMPILER = COMPILER_GNUC")
#elif defined( __BORLANDC__ )
#	undef COMPILER
#	define COMPILER					COMPILER_BORL
#	define COMP_VER					__BCPLUSPLUS__
#	define __FUNCTION__					__FUNC__
//#	pragma message("COMPILER = COMPILER_BORL")
#endif



//////////////////////////////////////////////////////////////////////////
// Determine target platform by compile environment macro.
//////////////////////////////////////////////////////////////////////////
#define TARGET_PLATFORM				PLATFORM_UNKNOWN

// win32
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WINDOWS)
#	undef  TARGET_PLATFORM
#	define TARGET_PLATFORM			PLATFORM_WIN32
#	pragma message("TARGET_PLATFORM = PLATFORM_WIN32")
#endif

// linux / android
#if defined(LINUX) || defined(linux) || defined(__linux__)
#	undef  TARGET_PLATFORM
#	if defined(ANDROID) || defined(__ANDROID__)
#		define TARGET_PLATFORM		PLATFORM_ANDROID
#		pragma message("TARGET_PLATFORM = PLATFORM_ANDROID")
#	else
#		define TARGET_PLATFORM		PLATFORM_LINUX
#		pragma message("TARGET_PLATFORM = PLATFORM_LINUX")
#	endif
#endif

// mac / ios
#if defined(__APPLE_CC__) || defined(APPLE) || defined(__MACH__)
#	undef  TARGET_PLATFORM
#	if (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000)
#		define TARGET_PLATFORM		PLATFORM_IOS
#		pragma message("TARGET_PLATFORM = PLATFORM_IOS")
#	else
#		define TARGET_PLATFORM		PLATFORM_MAC
#		pragma message("TARGET_PLATFORM = PLATFORM_MAC")
#	endif
#endif



//////////////////////////////////////////////////////////////////////////
// Find the architecture type
//////////////////////////////////////////////////////////////////////////
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#	define ARCH_TYPE				ARCHITECTURE_64
//#	pragma message("ARCH_TYPE = ARCHITECTURE_64")
#else
#	define ARCH_TYPE				ARCHITECTURE_32
//#	pragma message("ARCH_TYPE = ARCHITECTURE_32")
#endif


//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if (TARGET_PLATFORM == PLATFORM_UNKNOWN)
#error  "Cannot recognize the target platform, Abort!"
#endif

// check user set compiler
#if (COMPILER == COMPILER_NONE)
#error  "Cannot recognize the compiler, Abort!"
#endif

#if (TARGET_PLATFORM == PLATFORM_WIN32)
#	pragma warning (disable:4127)
#endif

#if (TARGET_PLATFORM == PLATFORM_WIN32)
#	if defined(STATIC_LIB)
#		define		CORE_API
#	else
#		if defined(CORE_EXPORTS) || defined(_SHARED_)
#			define	CORE_API				__declspec( dllexport )
#		else
#			define	CORE_API				__declspec( dllimport )
#		endif
#	endif
#else
#   if defined(STATIC_LIB)
#       define		CORE_API
#   else
#       if defined(CORE_EXPORTS) || defined(_SHARED_)
#           define  CORE_API             __attribute__((visibility("default")))
#		else
#			define	CORE_API
#		endif
#	endif
#endif

#if TARGET_PLATFORM == PLATFORM_ANDROID
#   include <stdio.h>
#   include <stdlib.h>
#   include <string>
#   include <jni.h>
#   include <android/log.h>
inline std::string get_tag( const char* file, int line )
{
    std::string tag(file);
    int pos = tag.find_last_of( "/\\" );
    if ( pos != -1 )
        tag = tag.substr( pos+1 );
    char buf[512];
    snprintf( buf, 512, "%s_%d", tag.c_str(), line );
    tag = buf;
    return tag;
}
#   define  DroidLOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, get_tag(__FILE__, __LINE__).c_str(),__VA_ARGS__);
#else
#   define  DroidLOGD(...)
#endif


#endif