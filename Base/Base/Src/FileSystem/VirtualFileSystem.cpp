#include "stdafx.h"
#include <map>
#include <set>
#include <list>
#include <string>
#include "Common.h"
#include "Stream.h"
#include "IFileSystem.h"
#include "MpkFileSystem.h"
#include "StdFileSystem.h"
#include "VirtualFileSystem.h"
#include "StringHash.h"

#if (TARGET_PLATFORM != PLATFORM_WIN32)
#include <pthread.h>
#endif

#define new RKT_NEW

namespace xs{

	FileSystem::~FileSystem()
	{
		for (StreamSystemVector::iterator iter = m_vStreamSystem.begin(); iter != m_vStreamSystem.end(); ++iter)
		{
			safeRelease(*iter);
		}
		m_vStreamSystem.clear();
	}

	static std::set<std::string> g_setFileSystemPath;
	IFileSystem* FileSystem::addFindPath(const char* pPath)
	{
		m_mutex.Lock();
		IFileSystem* streamSystem = openFileSystem(pPath);
		if(streamSystem)m_vStreamSystem.push_back(streamSystem);


		g_setFileSystemPath.insert(pPath);
		m_mutex.Unlock();

		return streamSystem;
	}

	Stream*	FileSystem::open(IFileSystem* pStreamSystem,const char* pFileName,const char* mode)
	{
		m_mutex.Lock();
		Stream *pStream = pStreamSystem->get(pFileName);
		if(pStream)
		{
			if(pStream->open(mode))
			{
				m_mutex.Unlock();
				return pStream;
			}
			else
			{
				safeRelease(pStream);
			}
		}
		m_mutex.Unlock();
		return 0;
	}

	Stream* FileSystem::open(const char* pFileName,const char* mode)
	{
		m_mutex.Lock();
		DWORD dwThreadId = GetCurrentThreadId();
		InfoLn("thread " << dwThreadId << " open " << pFileName);

		StreamSystemVector::const_iterator e = m_vStreamSystem.end();
		for(StreamSystemVector::const_iterator b = m_vStreamSystem.begin();b != e;++b)
		{
			//if(find(*b,pFileName))
			{
				Stream *pStream = (*b)->get(pFileName);
				if(pStream)
				{
					if(pStream->open(mode))
					{
						InfoLn("thread " << dwThreadId << " success open " << pFileName << " from " << (*b)->getFileSystemName());
						if (0 != strcmp("标准文件系统", (*b)->getFileSystemName()))
						{
							breakable;
						}
						m_mutex.Unlock();
						return pStream;
					}
					else
					{
						safeRelease(pStream);
					}
				}
			}
		}
		m_mutex.Unlock();
		ErrorLn("thread " << dwThreadId << " failed open " << pFileName);
		return 0;
	}

	bool		FileSystem::isExist(const char* pFileName)
	{
		m_mutex.Lock();
		StreamSystemVector::iterator e = m_vStreamSystem.end();
		for(StreamSystemVector::iterator b = m_vStreamSystem.begin();b != e;++b)
		{
			if((*b)->find(pFileName))
			{
				m_mutex.Unlock();
				return true;
			}
		}
		m_mutex.Unlock();
		return false;
	}

	void		FileSystem::list(IListFileCallback *pCallback,void *p)
	{
		m_mutex.Lock();
		StreamSystemVector::iterator e = m_vStreamSystem.end();
		for(StreamSystemVector::iterator b = m_vStreamSystem.begin();b != e;++b)
		{
			(*b)->list(pCallback,p);
		}
		m_mutex.Unlock();
	}


	CStreamHelper::CStreamHelper() : m_pStream(0)
	{
	}

	CStreamHelper::CStreamHelper(Stream* pStream) : m_pStream(pStream)
	{
	}

	CStreamHelper::~CStreamHelper()
	{
		close();
	}

	const CStreamHelper& CStreamHelper::operator=(Stream* pStream)
	{
		m_pStream = pStream;
		return *this;
	}

	//CStreamHelper::operator bool()
	//{
	//	return m_pStream != 0;
	//}

	Stream* CStreamHelper::operator->()
	{
		return m_pStream;
	}

	bool CStreamHelper::isNull()
	{
		return m_pStream == 0;
	}

	CStreamHelper::operator Stream*()
	{
		return m_pStream;
	}

	void CStreamHelper::close()
	{
		if(m_pStream)
		{
			m_pStream->close();
			safeRelease(m_pStream);
		}
	}
	
	RKT_API FileSystem*	getFileSystem()
	{
	
		//NOTE：目前假定线程不会退出，假定主线程会事先调用一次getFileSystem 然后addFindPath
		//经测试目前游戏客户端和服务器都是如此
		//因为使用文件系统的线程数较少最多只有4个，所以可以做成list用来做读时免锁,list的遍历效率问题也可以忽略
		//如果线程会退出则需要做好同步 及节点释放问题
		struct ThreadNode
		{
			DWORD dwThreadId;
			FileSystem* pFileSystem;
		};

		class FileSystemList:public std::list<ThreadNode>
		{
		public:
			~FileSystemList()
			{
				for (FileSystemList::iterator iter = begin(); iter != end(); iter++)
				{
					FileSystem* pFileSystem = (*iter).pFileSystem;
					delete pFileSystem;
				}
			}
		};
		static FileSystemList s_lstFileSystem;
		static FileSystem *s_pLastFileSystem = NULL;

		DWORD dwThreadId = GetCurrentThreadId();

		if (s_pLastFileSystem != NULL)
		{
			std::list<ThreadNode>::const_iterator itr = s_lstFileSystem.begin();
			while(true)
			{
				if (itr->dwThreadId == dwThreadId)
				{
					return itr->pFileSystem;
				}

				if (itr->pFileSystem == s_pLastFileSystem)
				{
					break;
				}
				else
				{
					itr++;
				}
			}
		}

		FileSystem * pFileSystem = new FileSystem;
		std::set<std::string>::iterator itr2 = g_setFileSystemPath.begin();
		for (; itr2 != g_setFileSystemPath.end(); itr2++)
		{
			pFileSystem->addFindPath(itr2->c_str());
		}

		//防止一开始两个线程同时push_back
		static Mutex	s_mutex;
		s_mutex.Lock();
		ThreadNode node;
		node.dwThreadId = dwThreadId;
		node.pFileSystem = pFileSystem;
		s_lstFileSystem.push_back(node);

		std::list<ThreadNode>::iterator itrTmp = --s_lstFileSystem.end();
		s_pLastFileSystem = itrTmp->pFileSystem;
		s_mutex.Unlock();

		ASSERT(s_lstFileSystem.size() < 5);
		return node.pFileSystem;
	}

	RKT_API IFileSystem*	openFileSystem(const char* pPath)
	{
		if(!pPath)return 0;

		IFileSystem* streamSystem = 0;
		CPathA path(pPath);
		toggleFullPath(path);

		if (isFile(path.c_str()))
		{
			std::string ext = path.substr(path.length() - 4,4);
			if(!strcmp(ext.c_str(),".PAK") || !strcmp(ext.c_str(),".MPQ"))
			{
				streamSystem = static_cast<IFileSystem*>(new MpkFileSystem);
			}
		}
		else if (isDirectory(path.c_str()))
			streamSystem = static_cast<IFileSystem*>(new StdFileSystem);

		if (!streamSystem || !streamSystem->open(path.c_str()))
		{
			safeRelease(streamSystem);
		}

		return streamSystem;
	}
}