//==========================================================================
/**
* @file	  : LoadRecourceThread.h
* @author : 
* created : 2008-1-21  22:32
* purpose : 
*/
//==========================================================================


#ifndef _LOADRESOURCETHREAD_H
#define _LOADRESOURCETHREAD_H
#include <queue>

namespace xs
{

	class ResourceNode;
	class ResourceThreadLoader;


	/** 任务状态
	*/
	enum stateTask
	{
		stateTaskEnd,   //任务结束
		stateTaskBusy,  //任务忙碌状态
		stateTaskIdle   //任务空闲状态
	};

	/** 线程加载任务类
	*/
	class LoadTask: public IRunnable
	{
		ResourceThreadLoader*		m_pResourceThreadLoader; //pointer of ResourceThreadLoader
		
		stateTask						m_stateTask;			//任务状态

		Semaphore						m_semaphore;			//信号量

		Semaphore						m_semaphoreInit;        //等待初始化完成

		bool                            m_bGroundPrivate;       //专门加载地表的

	public: //构造函数和析够函数
		LoadTask(ResourceThreadLoader*pResourceThreadLoader, bool bGroundPrivate = false):
			m_pResourceThreadLoader(pResourceThreadLoader),
			m_bGroundPrivate(bGroundPrivate),
			m_stateTask(stateTaskIdle),
			m_semaphore(false),
			m_semaphoreInit(false)
			
			{		
			}

		virtual ~LoadTask(){}

		//接口函数实现
	public:
		virtual void run();
		virtual void release();

	public: //调用函数
		//inline stateTask getStateTask() const { return m_stateTask; }
		inline bool post(){
			if(m_stateTask != stateTaskIdle)
				return false ;
			m_semaphore.Post() ;
			return true; 
		}

		void wait()
		{
			while(true)
			{
				if(m_semaphoreInit.Wait(100))
					break;
			}
		}

		bool IsGroundPrivate()
		{
			return m_bGroundPrivate;
		}
	};


	template<class _Ty>
	struct mygreater
		: public binary_function<_Ty, _Ty, bool>
	{	
		inline bool operator()(const _Ty& _Left, const _Ty& _Right) const
		{
			return _Left->getPriority() > _Right->getPriority();
		}
	};

	class ResourceThreadLoader
	{
		
		int									m_threadMaxCount;   //最多线程序数量  
		ThreadPool							m_threadPool;       //线程池
		bool								m_exitFlag;         //退出标志

		typedef std::priority_queue<ResourceNode*, std::vector<ResourceNode*>, mygreater<ResourceNode*> > QUEUE_NODES;
		
		QUEUE_NODES	m_queueNodes;
		QUEUE_NODES m_queueNodesGround;

		//std::queue<ResourceNode*>			m_queueNodes;		//待加载队列

		Mutex								m_mutexQueue;		//互斥对象
		Mutex								m_mutexQueueGround;	//地表加载互斥对象

		std::list<LoadTask*>				m_taskList;			//任务列表

		IRenderSystem*						m_pRenderSystem;

		uint								m_masterRenderTarget ;//主线程渲染系统的渲染目标
		

	public: //构造函数和析构函数
		ResourceThreadLoader():m_threadMaxCount(0),m_exitFlag(false),m_pRenderSystem(0){}
		virtual ~ResourceThreadLoader(){}

	public: //外部调用函数 
		/**创建线程
		@param threads 线程数量
		@return 成功返回true,失败返回false
		@note  目前只支持一个线程,这个与opengl多线程支持技术有关,以后需要完善
			   2010.12.13 zgz 引擎已启用D3D，现在是创建了2个线程，1个专用于地表加载
		*/
		bool create(int threads) ;

		/** 提交加载线程资源节点信息
		@node 线程资源节点信息
		*/
		void loadRes(ResourceNode*node);

		/** 为了复用代码，待删除的节点也由资源加载线程去释放 zgz
		@node 线程资源节点信息
		*/
		void releaseRes(ResourceNode* node);

		/** 关闭线程
		*/
		void close();

		/** 线程查询是否退出
		*/
		inline bool isExit(){ return m_exitFlag; }

		//取得一个加载节点
		ResourceNode* getNode(bool bGroundNode = false);

		IRenderSystem* getRenderSystem(){ return m_pRenderSystem ;}

		uint getMasterRenderTarget(){ return m_masterRenderTarget ;}

	};

	

}


#endif