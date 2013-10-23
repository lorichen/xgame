/**
* Created by pk 2008.01.02
*/

#include "stdafx.h"
#include "Trace.h"
#include "Thread.h"
#include "StringHelper.h"
#include "path.h"

#include <list>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#define new RKT_NEW

#ifdef RKT_WIN32
//#	include <tchar.h>
#	include <windows.h>
#	include <richedit.h>
#endif

namespace xs {

	const char* getCurTimeString()
	{
		static char buf[64] = {0};

		buf[0] = 0;
		time_t t = time(NULL);
		struct tm* local = localtime(&t);

		strftime(buf, 64, "[%H:%M:%S] ", local);
		return buf;
	}

	class TraceConsole : public TraceListener
	{
	public:
		virtual void onTrace(const char* msg, const char* time, TraceLevel level)
		{
			assert(msg != NULL);

			if (time && hasTime())
				std::cout<<time;
			std::cout<<msg;
		}
	};

	class TraceHtmlFile : public TraceListener
	{
		FILE* mFile;
	public:
		TraceHtmlFile() : mFile(0) { }
		bool create(const char* filename)
		{
			mFile = fopen(filename, "wt");
			if (!mFile) return false;
			assert(mFile != 0);

			fputs(
				"<html>\n"
				"<head>\n"
				"<title>Html Output</title>"
				"</head>\n"
				"<body>\n<font face=\"Fixedsys\" size=\"2\" color=\"#0000FF\">\n", mFile);
			return true;
		}
		~TraceHtmlFile()
		{
			if (mFile)
			{
				fputs("</font>\n</body>\n</html>", mFile);
				fclose(mFile);
			}
		}

		virtual void onTrace(const char* msg, const char* time, TraceLevel level)
		{
			assert(msg != NULL);

			static const char* color[] = 
			{
				0,
				"<font color=\"#000000\">", // Info
				"<font color=\"#0000FF\">",	// Trace
				0,
				"<font color=\"#FF00FF\">",	// Warning
				0,0,0,
				"<font color=\"#FF0000\">"	// Error
			};

			fputs(color[(int)level], mFile);

			// 输出时间
			if (time && hasTime())
				fputs(time, mFile);

			// 输出正文(遇\n即换行)
			char* pStart = (char*)msg;
			char* pPos = pStart;
			char* pEnd = pStart + strlen(msg) - sizeof(char);

			while (pPos <= pEnd)
			{
				if (*pPos == '\n') // 换行
				{
					if (pStart < pPos)
						fwrite(pStart, pPos - pStart, 1, mFile);
					fputs("<br>", mFile);

					pStart = ++pPos;
				}
				else
				{
					pPos ++;
				}
			}

			// 尾部的部分
			if (pStart < pPos)
				fwrite(pStart, pPos - pStart, 1, mFile);

			fputs("</font>\n", mFile);// 一条信息结束，加个硬换行

			fflush(mFile);
		}
	};

#if 0
	class TraceFile : public TraceListener
	{
		HANDLE                  m_hFile;        // Output file's handle.
		const DWORD             m_dwViewSize;   // Size of the mapped view of the output file.
		LPSTR                   m_pView;        // The mapped view of the output file, or NULL
		// if the output is the standard output.
		DWORD                   m_dwViewNum;    // View number.
		// or 0 if the output is the standard output.
		DWORD                   m_dwOffset;     // Current offset to which data must be written,
		// or 0 if the output is the standard output.
	public:
		TraceFile()         
			: m_pView(NULL), m_dwViewSize(10485760), m_dwOffset(0),
			m_dwViewNum(0) { }
		bool create(const char* filename)
		{
			m_hFile = CreateFile(filename, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
			if(m_hFile != INVALID_HANDLE_VALUE)
			{
				HANDLE hMap = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, m_dwViewSize, NULL);
				if(hMap != NULL)
				{
					m_pView = (LPSTR)MapViewOfFile(hMap, FILE_MAP_WRITE/*FILE_MAP_ALL_ACCESS*/, 0, 0, 0);
					if(m_pView == NULL)
					{
						CloseHandle(m_hFile);
						m_hFile = NULL;
						return false;
					}
					CloseHandle(hMap);
				}
				else
				{
					CloseHandle(m_hFile);
					m_hFile = NULL;
					return false;
				}
			}
			else
			{
				m_hFile = NULL;
				return false;
			}

			return true;
		}
		~TraceFile()
		{
			if(m_pView && m_hFile)
			{
				UnmapViewOfFile(m_pView);
				SetFilePointer(m_hFile, m_dwViewNum * m_dwViewSize + m_dwOffset, NULL, FILE_BEGIN);
				SetEndOfFile(m_hFile);
				CloseHandle(m_hFile);
			}
		}

		virtual void onTrace(const char* msg, const char* time, TraceLevel level)
		{
			Write(msg);
		}

		void Write(const char* zString)
		{
			DWORD dwLen = lstrlen(zString);
			Write(zString, dwLen);
		}

		void Write(const char* zString, DWORD dwLen)
		{
			if(m_pView == NULL)
			{
				if(m_hFile != NULL)
				{
					//// Output to standard output.

					DWORD dw;
					WriteFile(m_hFile, zString, dwLen, &dw, NULL);
				}
			}
			else
			{
				//// Output to file.

				BOOL bNewView = m_dwOffset + dwLen > m_dwViewSize;
				DWORD dwToWrite = bNewView ? m_dwViewSize - m_dwOffset : dwLen;

				if(dwToWrite > 0)
					::CopyMemory(m_pView + m_dwOffset, (CONST VOID*)zString, dwToWrite);

				// If a new view is needed, then replace the old one.
				if(bNewView)
				{
					// Must enlarge the file and map a new view.
					UnmapViewOfFile(m_pView);
					DWORD dwCurrentFileSize = GetFileSize(m_hFile, NULL);
					HANDLE hMap = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, dwCurrentFileSize + m_dwViewSize, NULL);
					if(hMap != NULL)
					{
						m_pView = (LPSTR)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, dwCurrentFileSize, m_dwViewSize);
						if(m_pView == NULL)
						{
							CloseHandle(m_hFile);
							m_hFile = NULL;
						}
						else
						{
							// Write the next chunk.
							m_dwViewNum++;
							::CopyMemory(m_pView, (CONST VOID*)(zString + dwToWrite), dwLen - dwToWrite);
							m_dwOffset = dwLen - dwToWrite;
						}
						CloseHandle(hMap);
					}
					else
					{
						CloseHandle(m_hFile);
						m_hFile = NULL;
					}
				}
				else
					m_dwOffset += dwToWrite;
			}
		}
	};
#endif

#ifdef _DEBUG
#	define MAX_RICHEDIT_MESSAGE_LEN	(256 * 1024)		/// RichEdit中最大容纳长度
#else // _DEBUG
#	define MAX_RICHEDIT_MESSAGE_LEN	(128 * 1024)
#endif

	RKT_API void addTraceToRichEdit(void* hWndRichEdit, const char* msg, TraceLevel level)
	{
#ifdef RKT_WIN32
		assert(msg != NULL);

		HWND hWnd = (HWND)hWndRichEdit;
		if (hWnd == NULL || !::IsWindow(hWnd))
			return;

		// GetSel
		CHARRANGE crOld;
		::SendMessage(hWnd, EM_EXGETSEL, 0, (LPARAM)&crOld);

		// GetTextLength
		int nLen = (int)::SendMessage(hWnd, WM_GETTEXTLENGTH, NULL, NULL);
		int nStrLen = (int)strlen(msg);
		CHARRANGE cr;
		if (nLen + nStrLen > MAX_RICHEDIT_MESSAGE_LEN)
		{
			// SetSel
			cr.cpMin = 0;
			cr.cpMax = nLen + nStrLen - MAX_RICHEDIT_MESSAGE_LEN; //+ (MAX_RICHEDIT_MESSAGE_LEN>>5);
			::SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&cr);
			// ReplaceSel
			::SendMessage(hWnd, EM_REPLACESEL, (WPARAM)0, (LPARAM)"");
			// GetTextLength
			nLen = (int)::SendMessage(hWnd, WM_GETTEXTLENGTH, NULL, NULL);
		}

		// SetSel
		cr.cpMin = nLen;
		cr.cpMax = nLen;
		::SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&cr);

		// SetSelectionCharFormat
		CHARFORMAT2 cf;
		memset(&cf, 0, sizeof(CHARFORMAT2));
		cf.dwMask = CFM_COLOR | CFM_CHARSET | CFM_SIZE 
			| CFM_BOLD | CFM_ITALIC | CFM_STRIKEOUT |  CFM_UNDERLINE | CFM_LINK | CFM_SHADOW;
		cf.dwEffects = 0;
		cf.bCharSet = GB2312_CHARSET;
		const COLORREF cls[] = {0, RGB(0,0,0), RGB(0,0,255), 0, RGB(255,0,255), 0,0,0,RGB(255,0,0)};
		cf.crTextColor = cls[(int)level];
		cf.yHeight = 9 * 20;
		cf.cbSize = sizeof(CHARFORMAT);
		::SendMessage(hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

		// ReplaceSel
		::SendMessage(hWnd, EM_REPLACESEL, (WPARAM) 0, (LPARAM)msg);

		if (crOld.cpMax > crOld.cpMin)
		{
			// SetSel
			::SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&crOld);
		}

		// Scroll lines
		SCROLLINFO ScrollInfo;
		ScrollInfo.cbSize = sizeof(SCROLLINFO);
		ScrollInfo.fMask = SIF_ALL;
		::GetScrollInfo(hWnd, SB_VERT, &ScrollInfo);

		int nTotalLine = (int)::SendMessage(hWnd, EM_GETLINECOUNT, 0, 0);
		if (nTotalLine > 0)
		{
			int nEachLineHeihgt = ScrollInfo.nMax / nTotalLine;
			if (nEachLineHeihgt > 0)
			{
				int nUpLine = 0;
				if (nTotalLine > 0 && ScrollInfo.nMax > 0 && nEachLineHeihgt > 0)
					nUpLine = (ScrollInfo.nMax - ScrollInfo.nPos - (ScrollInfo.nPage - 1)) / nEachLineHeihgt;
				if (nUpLine > 0)
					::SendMessage(hWnd, EM_LINESCROLL, 0, nUpLine);
			}
		}
#endif // RKT_WIN32
	}

	class TraceRichEdit : public TraceListener
	{
		void* mHwnd;
		struct _MSG
		{
			std::string msg;
			TraceLevel level;
		};
		std::list<_MSG>	msgs;
		Mutex mutex;
	public:
		TraceRichEdit(void* hwnd) : mHwnd(hwnd) {}
		virtual void onTrace(const char* msg, const char* time, TraceLevel level)
		{
			MultiThread::scopelock lock(mutex);
			if (!mHwnd) return;
			msgs.push_back(_MSG());
			_MSG& record = msgs.back();
			record.level = level;
			if (time && hasTime())
				record.msg += time;
			record.msg += msg;
/*
			if (time && hasTime())
				addTraceToRichEdit(mHwnd, time, level);
			addTraceToRichEdit(mHwnd, msg, level);*/
		}
		void dispatch()
		{
			MultiThread::scopelock lock(mutex);
			while (!msgs.empty())
			{
				_MSG& record = msgs.front();
				addTraceToRichEdit(mHwnd, record.msg.c_str(), record.level);
				msgs.erase(msgs.begin());
			}
		}
	};

	struct STrace
	{
		typedef std::list<TraceListener*>	ListenerList;
		ListenerList	sinks;		/// 接收器列表
		Mutex*			mutex;		/// 互斥体
		int				level;		/// 输出级别
		bool			hasTime;	/// 是否显示时间
	};

	STrace* gTrace = 0;

	RKT_API void closeTrace()
	{
		if (gTrace)
		{
			if (gTrace->mutex)
			{
				gTrace->mutex->Lock();
				gTrace->mutex->Unlock();
				delete gTrace->mutex;
			}
			for (STrace::ListenerList::iterator it=gTrace->sinks.begin(); it!=gTrace->sinks.end(); ++it)
				delete *it;
			delete gTrace;
			gTrace = 0;
		}
	}

	RKT_API void createTrace(int level, bool hasTime, bool threadSafe)
	{
		closeTrace();

		gTrace = new STrace;
		gTrace->level = level;
		gTrace->mutex = threadSafe ? new Mutex() : 0;
		gTrace->hasTime = hasTime;
	}

	RKT_API int setTraceLevel(int level)
	{
		if (!gTrace) createTrace();
		int old = gTrace->level;
		gTrace->level = level;
		return old;
	}

	RKT_API bool setTraceHasTime(bool hasTime)
	{
		if (!gTrace) createTrace();
		bool old = gTrace->hasTime;
		gTrace->hasTime = hasTime;
		return old;
	}

	RKT_API void registerTrace(TraceListener* sink)
	{
		if (!sink) return;
		if (!gTrace) createTrace();
		gTrace->sinks.push_back(sink);
	}

	RKT_API void unregisterTrace(TraceListener* sink)
	{
		if (!sink || !gTrace) return;
		for (STrace::ListenerList::iterator it=gTrace->sinks.begin(); it!=gTrace->sinks.end(); ++it)
		{
			if (*it == sink)
			{
				gTrace->sinks.erase(it);
				return;
			}
		}
	}

	RKT_API TraceListener* output2Console(int level, bool hasTime)
	{
		if (!gTrace) createTrace();
		TraceConsole* sink = new TraceConsole();
		sink->setTraceLevel(level);
		sink->setHasTime(hasTime);
		gTrace->sinks.push_back(sink);
		return sink;
	}

	RKT_API TraceListener* output2File(const char* filename, int level, bool hasTime)
	{
		char fn[256];
		if (!filename)
		{
			sprintf(fn, "%s\\log.html", getWorkDir());
			filename = fn;
		}
		if (filename)
		{
			if (!gTrace) createTrace();
			TraceHtmlFile* sink = new TraceHtmlFile();
			if (!sink->create(filename))
			{
				delete sink;
				return 0;
			}
			sink->setTraceLevel(level);
			sink->setHasTime(hasTime);
			gTrace->sinks.push_back(sink);
			return sink;
		}
		return 0;
	}

	RKT_API TraceListener* output2RichEdit(void* hwnd, int level, bool hasTime)
	{
#ifdef RKT_WIN32
		if (!gTrace) createTrace();
		TraceRichEdit* sink = new TraceRichEdit(hwnd);
		sink->setTraceLevel(level);
		sink->setHasTime(hasTime);
		gTrace->sinks.push_back(sink);
		return sink;
#else
		return 0;
#endif
	}

	RKT_API TraceListener* output2Socket(const char* host, ushort port, bool hasTime)
	{
		return 0;
		if (host && port != 0)
		{
			if (!gTrace) createTrace();
		}
	}

	RKT_API void output(const char* msg, TraceLevel level, const char* file, int line)
	{
		if (!gTrace)
		{
#ifdef RKT_DEBUG
			OutputDebugStringA(file);
			char lineInfo[32] = {0};
			sprintf(lineInfo, "(%d): ", line);
			OutputDebugStringA(lineInfo);
			OutputDebugStringA(msg);
#endif
			return;//createTrace();
		}

		if ((gTrace->level & (int)level) == 0)
			return;

		if (msg)
		{
			if (gTrace->mutex)
			{
#ifdef RKT_DEBUG
				gTrace->mutex->Lock(file, line);
#else
				gTrace->mutex->Lock();
#endif
			}

			const char* time = gTrace->hasTime ? getCurTimeString() : 0;

#ifdef RKT_DEBUG
			OutputDebugStringA(file);
			char lineInfo[32] = {0};
			sprintf(lineInfo, "(%d): ", line);
			OutputDebugStringA(lineInfo);
			OutputDebugStringA(msg);
#endif
			for (STrace::ListenerList::iterator it=gTrace->sinks.begin(); 
				it!=gTrace->sinks.end(); ++it)
			{
				if ((*it)->getTraceLevel() & level)
					(*it)->onTrace(msg, time, level);
			}

			if (gTrace->mutex)
				gTrace->mutex->Unlock();
		}
	}

	RKT_API void dispatch2RichEdit(TraceListener* tl)
	{
		TraceRichEdit* tre = (TraceRichEdit*)tl;
		if (tre)
		{
			tre->dispatch();
		}
	}

}