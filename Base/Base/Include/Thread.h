// unl_thread.h
#ifndef RKT_THREAD_H
#define RKT_THREAD_H

#include "Trace.h"
#include <vector>

namespace xs {

// Create thread
#ifdef WIN32
typedef unsigned int (WINAPI * _FN_THREAD_ROUTINE)(void* pvContext);
typedef _FN_THREAD_ROUTINE FN_THREAD_ROUTINE;

#define beginThread(psa, iStack, fnStartAddr, \
	pvContext, dwCreate, piThreadId) \
	((HANDLE)::_beginthreadex( \
	(void*)						(psa), \
	(unsigned)					(iStack), \
	(xs::FN_THREAD_ROUTINE)	(fnStartAddr), \
	(void*)						(pvContext), \
	(unsigned)					(dwCreate), \
	(unsigned*)					(piThreadId)))
#else
typedef void* (SYS_API * _FN_THREAD_ROUTINE)(void* pvContext);
typedef _FN_THREAD_ROUTINE FN_THREAD_ROUTINE;

#define beginThread(pThreadId, pAttr, fnStartAddr, pvContext) \
	::pthread_create( \
								pThreadId, \
								pAttr, \
	(xs::FN_THREAD_ROUTINE)	(fnStartAddr), \
	(void*)						(pvContext))
#endif

#define BEGIN_THREAD	beginThread // for compatible

// 临界区线程安全辅助类
#ifdef WIN32
// 互斥体
class Mutex
{
	CRITICAL_SECTION m_cs;

#ifdef RKT_DEBUG
	const char* file;
	int			line;
#endif

public:
	Mutex(bool = true){ ::InitializeCriticalSection(&m_cs); 
#ifdef RKT_DEBUG
	file = 0, line = 0;
#endif
	}
	~Mutex(){ ::DeleteCriticalSection(&m_cs); }
#ifdef RKT_DEBUG
	void Lock(const char* f=0, int l=0)		{ ::EnterCriticalSection(&m_cs); file=f; line=l; }
	void Unlock()						{ file=0; line=0; ::LeaveCriticalSection(&m_cs); }
#else
	void Lock()		{ ::EnterCriticalSection(&m_cs); }
	void Unlock()	{ ::LeaveCriticalSection(&m_cs); }
#endif
//	bool trylock(){ return ::TryEnterCriticalSection(&m_cs); }
};
#else
class Mutex
{
public:
	Mutex(bool recursive = true)
	{
		if(recursive)
		{
			pthread_mutexattr_t attr;
			::pthread_mutexattr_init(&attr);
			::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
			::pthread_mutex_init(&m_mutex, &attr);
			::pthread_mutexattr_destroy(&attr);
		}
		else
		{
			::pthread_mutex_init(&m_mutex, NULL);
		}
	}
	~Mutex(){ ::pthread_mutex_destroy(&m_mutex); }
	void Lock(){ ::pthread_mutex_lock(&m_mutex); }
	void Unlock(){ ::pthread_mutex_unlock(&m_mutex); }
//	bool trylock(){ return ::pthread_mutex_trylock(&m_mutex) == 0; }
private:
	pthread_mutex_t m_mutex;
};
#endif



/// 伪装的（空的）互斥体
class FakeMutex
{
#ifdef RKT_DEBUG
public:
	const char* file;
	int			line;
#endif
public:
	FakeMutex(bool = true){}
#ifdef RKT_DEBUG
	void Lock(const char* =0, int =0)	{}
#else
	void Lock()		{}
#endif
	void Unlock()	{}
};



/// 临界区线程安全自动保护
template<class _Mutex = Mutex>
class ResGuard
{
	_Mutex& mMutex;
public:
#ifdef RKT_DEBUG
	ResGuard(_Mutex& mtx, const char* f=0, int l=0) : mMutex(mtx){ mMutex.Lock(f, l); }
	~ResGuard()							{ mMutex.Unlock(); }
#else
	ResGuard(_Mutex& mtx) : mMutex(mtx)	{ mMutex.Lock(); }
	~ResGuard()							{ mMutex.Unlock(); }
#endif
};

// 单线程模型，自增减没有互斥处理
class SingleThread
{
public:
	static long _Increment(long* p){ return ++(*p); }
	static long _Decrement(long* p){ return --(*p); }
	typedef FakeMutex			mutex;
	typedef ResGuard<FakeMutex>	scopelock;
};
// 多线程模型，自增减互斥处理
class MultiThread
{
public:
	static long _Increment(long* p){ return ::InterlockedIncrement(p); }
	static long _Decrement(long* p){ return ::InterlockedDecrement(p); }
	typedef Mutex				mutex;
	typedef ResGuard<Mutex>		scopelock;
};

#ifdef WIN32
// 信号量
class Semaphore
{
	HANDLE m_h;
public:
	Semaphore(int initCount = 0)
	{
		m_h = ::CreateSemaphore(NULL, initCount, 0x7fffffff, NULL);	
	}
	~Semaphore(){ if(m_h) ::CloseHandle(m_h); }
	BOOL Wait(ULONG timeOut = INFINITE)
	{		
		DWORD ret = ::WaitForSingleObject(m_h, timeOut);
		if(ret == WAIT_OBJECT_0) 
		{
			return TRUE;
		}
		
		return FALSE;
	}
	void Post(){ BOOL bret = ::ReleaseSemaphore(m_h, 1, NULL); }
};
#else
class Semaphore
{
	sem_t m_sem;
public:
	Semaphore(int initCount = 0)
	{
		int ret = ::sem_init(&m_sem, 0, initCount);		
	}
	~Semaphore(){ ::sem_destroy(&m_sem); }
	BOOL Wait(ULONG timeOut = 0xffffffff)
	{
		int ret = 0;
		if(timeOut == 0xffffffff)
		{
			while(1)
			{
				ret = ::sem_wait(&m_sem);
				if(ret == 0) 
				{
					return TRUE;
				}
				
			}
		}
		else
		{
			timespec ts;
			ret = ::clock_gettime(CLOCK_REALTIME, &ts);			
			ts.tv_sec += timeOut / 1000;
			ts.tv_nsec += (timeOut % 1000) * 1000000;
			if(ts.tv_nsec >= 1000000000)
			{
				ts.tv_sec += 1;
				ts.tv_nsec -= 1000000000;
			}
			while(1)
			{
				ret = ::sem_timedwait(&m_sem, &ts);
				if(ret == 0) return TRUE;
				if(::GetLastError() == EINTR) 
				{
					continue;
				}				
				return FALSE;
			}
		}

		return FALSE;
	}
	void Post(){ int ret = ::sem_post(&m_sem); }
};
#endif


	/// 可运行对象
	struct IRunnable
	{
		virtual void run() = 0;
		virtual void release() = 0;
	};


	/// 简单线程封装
	class Thread
	{
	protected:
		DWORD	mThreadId;
		HANDLE	mThreadHandle;

	public:
		Thread() : mThreadId(0), mThreadHandle(NULL)
		{
		}

		~Thread()
		{
		}

	public:
		HANDLE spawn(IRunnable* task, int priority = THREAD_PRIORITY_NORMAL) 
		{
#if defined(HAVE_BEGINTHREADEX)
			mThreadHandle = (HANDLE)::_beginthreadex(0, 0, &_dispatch, task, 0, (unsigned int*)&mThreadId);
#else
			mThreadHandle = ::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&_dispatch, task, 0, (DWORD*)&mThreadId);
			if (NULL != mThreadHandle)
			{
				SetThreadPriority(mThreadHandle, priority);
			}			
#endif
			//printf("Task(%d):%s\n", mThreadId, task->getName());
			return mThreadHandle;
		}

		void wait()
		{
			if(mThreadHandle == NULL)
				return;
			::WaitForSingleObject(mThreadHandle, INFINITE);

			mThreadHandle = NULL;
		}

		void start()
		{
			::ResumeThread(mThreadHandle);
		}

		void pause()
		{
			::SuspendThread(mThreadHandle);
		}

		static void sleep(unsigned long timeout)
		{
			Sleep(timeout);
		}

	protected:
		static unsigned int __stdcall _dispatch(void* arg)
		{
			IRunnable* task = reinterpret_cast<IRunnable*>(arg);
			Assert(task);
#if  !defined(RELEASEDEBUG) && !defined(_DEBUG)
			try
#endif
			{

				task->run();				
			}
#if  !defined(RELEASEDEBUG) && !defined(_DEBUG)
			catch(...)
			{
				Error("Thread("<<::GetCurrentThreadId()<<") catch error"<<endl);
			}
#endif
			task->release();

#if defined(HAVE_BEGINTHREADEX)
			::_endthreadex(0);
#else
			ExitThread(0);
#endif

			return 0;
		}
	};



	/// 简单线程池
	class ThreadPool
	{
	protected:
		typedef std::vector<Thread*>	ThreadList;
		ThreadList	mThreads;
		bool mAutoRemove;	/// 当工作线程执行结束后，自动删除

	public:
		ThreadPool::ThreadPool(void)
		{
		}

		ThreadPool::~ThreadPool(void)
		{
			clear();
		}

		HANDLE ThreadPool::add(IRunnable* task, int priority = THREAD_PRIORITY_NORMAL)
		{
			Thread* thread = new Thread();
			mThreads.push_back(thread);
			return	thread->spawn(task, priority);
		}

		void ThreadPool::start()
		{
			for (ThreadList::iterator it=mThreads.begin(); it!=mThreads.end(); ++it)
			{
				(*it)->start();
			}
		}

		void ThreadPool::pause()
		{
			for (ThreadList::iterator it=mThreads.begin(); it!=mThreads.end(); ++it)
			{
				(*it)->pause();
			}
		}

		void ThreadPool::wait()
		{
			for (ThreadList::iterator it=mThreads.begin(); it!=mThreads.end(); ++it)
			{
				(*it)->wait();
			}
		}

		void ThreadPool::clear()
		{
			for (ThreadList::iterator it=mThreads.begin(); it!=mThreads.end(); ++it)
			{
				delete *it;
			}

			mThreads.clear();
		}

		size_t getThreadCount() const	{ return mThreads.size(); }
	};

} // namespace

#endif // RKT_THREAD_H
