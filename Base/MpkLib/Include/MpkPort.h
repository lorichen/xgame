#ifndef __MPKPORT_H__
#define __MPKPORT_H__

// Defines for Windows
#if !defined(__PLATFORM__) && defined(WIN32)

  // In MSVC 8.0, there are some functions declared as deprecated.
  #if _MSC_VER >= 1400
//  #define _CRT_SECURE_NO_DEPRECATE
  #define _CRT_NON_CONFORMING_SWPRINTFS
  #endif

  #include <stdio.h>      
  #include <windows.h>      
  #define   LITTLEENDIANPROCESSOR  1
  #define __PLATFORM__                  // The platform is known now

#endif

// Defines for Windows
#if !defined(__PLATFORM__) && defined(__APPLE__)  // Mac Carbon API

  // Macintosh using Carbon
  #if defined(__MACH__)
    //#include <Carbon/Carbon.h> // Mac OS X
    #define stricmp strcasecmp  // Case insensitive strcmp has a different name on this platform.
  #else
    #include <Carbon.h> // Mac OS 9 Using Carbon Lib
  #endif
  
  typedef void          * LPCSTR;
  typedef unsigned long * LPDWORD;
  typedef long          * PLONG;
  typedef void          * LPVOID;
  typedef unsigned int  UINT;
  
  #define    PKEXPORT
  #define    __SYS_ZLIB
  #define    LANG_NEUTRAL   0

  #define    LITTLEENDIANPROCESSOR  0
  #define __PLATFORM__                  // The platform is known now

#endif

// Assumption: we are not on Windows nor Macintosh, so this must be linux *grin*
// Ladik : Why the hell Linux does not use some OS-dependent #define ?
#if !defined(__PLATFORM__)

  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <stdint.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include <ctype.h>

  #define   LITTLEENDIANPROCESSOR   1
  #define __PLATFORM__

#endif  /* not __powerc */


#ifndef WIN32

  // Typedefs for ANSI C
  typedef unsigned char  BYTE;
  typedef short          SHORT;
  typedef unsigned short WORD;
  typedef long           LONG;
  //typedef unsigned long  DWORD;
#ifndef __OBJC__
  #define BOOL           bool
#endif
  typedef void         * HANDLE;
  typedef void         * LPOVERLAPPED; // Unsupported on Linux
  typedef char           TCHAR;
  typedef unsigned long  LCID;
  
  typedef struct _FILETIME
  { 
      DWORD dwLowDateTime; 
      DWORD dwHighDateTime; 
  }
  FILETIME, *PFILETIME; 

  // Some Windows-specific defines
  #ifndef MAX_PATH
    #define MAX_PATH 1024
  #endif

  #ifndef TRUE
    #define TRUE true
  #endif

  #ifndef FALSE
    #define FALSE false
  #endif

  #define VOID     void
  #define WINAPI 

  #define FILE_BEGIN    SEEK_SET
  #define FILE_CURRENT  SEEK_CUR
  #define FILE_END      SEEK_END
  
  #define CREATE_NEW    1
  #define CREATE_ALWAYS 2
  #define OPEN_EXISTING 3
  #define OPEN_ALWAYS   4
  
  #define FILE_SHARE_READ 0x00000001L
  #define GENERIC_WRITE   0x40000000
  #define GENERIC_READ    0x80000000
  
  #define FILE_FLAG_DELETE_ON_CLOSE     1   // Sam: Added these two defines so it would compile.
  #define FILE_FLAG_SEQUENTIAL_SCAN     2
  
  #define ERROR_SUCCESS                 0
  #define ERROR_INVALID_FUNCTION        1
  #define ERROR_FILE_NOT_FOUND          2
  #define ERROR_ACCESS_DENIED           5
  #define ERROR_NOT_ENOUGH_MEMORY       8
  #define ERROR_BAD_FORMAT             11
  #define ERROR_NO_MORE_FILES          18
  #define ERROR_HANDLE_EOF             38
  #define ERROR_HANDLE_DISK_FULL       39
  #define ERROR_INVALID_PARAMETER      87
  #define ERROR_DISK_FULL             112
  #define ERROR_CALL_NOT_IMPLEMENTED  120
  #define ERROR_ALREADY_EXISTS        183
  #define ERROR_CAN_NOT_COMPLETE     1003
  #define ERROR_FILE_CORRUPT         1392
  #define ERROR_INSUFFICIENT_BUFFER  4999
  
  #define INVALID_HANDLE_VALUE ((HANDLE) -1)
  
  #ifndef min
  #define min(a, b) ((a < b) ? a : b)
  #endif
  
  #ifndef max
  #define max(a, b) ((a > b) ? a : b)
  #endif
  
  extern int globalerr;
  
  void  SetLastError(int err);
  int   GetLastError();
  char *ErrString(int err);

  // Emulation of functions for file I/O available in Win32
  HANDLE CreateFile(const char * lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void * lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
  BOOL   CloseHandle(HANDLE hObject);

  DWORD  GetFileSize(HANDLE hFile, DWORD * lpFileSizeHigh);
  DWORD  SetFilePointer(HANDLE, LONG lDistanceToMove, LONG * lpDistanceToMoveHigh, DWORD dwMoveMethod);
  BOOL   SetEndOfFile(HANDLE hFile);

  BOOL   ReadFile(HANDLE hFile, void * lpBuffer, DWORD nNumberOfBytesToRead, DWORD * lpNumberOfBytesRead, void * lpOverLapped);
  BOOL   WriteFile(HANDLE hFile, const void * lpBuffer, DWORD nNumberOfBytesToWrite, DWORD * lpNumberOfBytesWritten, void * lpOverLapped);

  BOOL   IsBadReadPtr(const void * ptr, int size);
  DWORD  GetFileAttributes(const char * szileName);

  BOOL   DeleteFile(const char * lpFileName);
  BOOL   MoveFile(const char * lpFromFileName, const char * lpToFileName);
  void   GetTempPath(DWORD szTempLength, char * szTemp);
  void   GetTempFileName(const char * lpTempFolderPath, const char * lpFileName, DWORD something, char * szLFName);

  #define strnicmp strncasecmp

#endif // !WIN32

#if LITTLEENDIANPROCESSOR
    #define    LITTLEENDIAN16BITS(a)                        (a)
    #define    LITTLEENDIAN32BITS(a)                        (a)
    #define    CONVERTBUFFERTOLITTLEENDIAN32BITS(a,b)       {}
    #define    CONVERTBUFFERTOLITTLEENDIAN16BITS(a,b)       {}
    #define    CONVERTTMPKHEADERTOLITTLEENDIAN(a)           {}
#else
    extern unsigned short SwapShort(unsigned short);
    extern unsigned long SwapLong(unsigned long);
    extern void ConvertUnsignedLongBuffer(unsigned long *buffer, unsigned long nbLongs);
    extern void ConvertUnsignedShortBuffer(unsigned short *buffer, unsigned long nbShorts);
    extern void ConvertTMPKHeader(void *header);
    #define    LITTLEENDIAN16BITS(a)    SwapShort((a))
    #define    LITTLEENDIAN32BITS(a)    SwapLong((a))
    #define    CONVERTBUFFERTOLITTLEENDIAN32BITS(a,b)        ConvertUnsignedLongBuffer((a),(b))
    #define    CONVERTBUFFERTOLITTLEENDIAN16BITS(a,b)        ConvertUnsignedShortBuffer((a),(b))
    #define    CONVERTTMPKHEADERTOLITTLEENDIAN(a)            ConvertTMPKHeader((a))
#endif

#endif // __MPKPORT_H__
