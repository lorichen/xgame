//==========================================================================
/**
* @file	  : ResPool.h
* @author : 
* created : 2008-1-18   13:46
* purpose : 
*/
//==========================================================================

#ifndef __RESPOOL_H__
#define __RESPOOL_H__

#include"IResourceManager.h"
#include "Common.h"
#include <hash_map>
#include <list>

/**
	资源池，用于缓存系统的资源，这些资源一般是数量很多，而且在运行时不进行修改
*/

namespace xs {

	/// 池节点
	template<class KeyType>
	class PoolNode
	{
		KeyType			mId;
		int				mRefs;
	
	public:
		PoolNode(){ mRefs = 0; }
		/// 获取对象所占内存的大小
		virtual ulong getSize() const = 0;

		/// 每个对象都有一个键值与之对应
		virtual KeyType getKey() const = 0;

		/// 释放对象(不考虑引用计数)
		virtual void release() = 0;

		virtual ResourceState getState() const = 0 ;

		/// 得到节点的引用次数
		int getRefCount() const							{ return mRefs; }

		/// 添加一次引用
		int incRef()									{ return ++mRefs; }

		/// 减去一次引用(考虑到缓冲池的可回收性，这里允许引用计数为0时不进行release操作)
		int decRef()									{ if(--mRefs<=0) mRefs=0;return mRefs; }
	};

	/** 资源池
	* 输入：
	* 　	PoolNodeType	：必须继承自PoolNode或实现PoolNode的所有功能
	*		KeyType			：键类型，如ulong,__int64等

	* 思路：
	* 采用引用计数, 缓冲池的容量可由外部动态设置(默认128M)
	* 两个 list : 一个标准列表，记录正在使用的节点；一个可回收列表，记录被扔掉的节点, 利用了list的快速增删操作
	* 节点的查询通过hash_map快速查询，hash_map记录了各节点在两个list中的位置，利用了hash_map的快速搜索操作
	* 当计数为0时，不删除，放到可回收list中
	* 2011.3.24 zgz
	* 修改为3个list： 一个标准列表，记录正在使用的节点；一个可回收列表，记录被扔掉的节点；一个延迟回收列表，此列表
	* 中的节点至少保持2分钟，避免频繁使用的资源被释放
	*/
	template <class PoolNodeType, class KeyType>
	class ResPool
	{
	private:
		typedef  typename std::list<PoolNodeType>				NodeList;
		typedef HashMap<KeyType, typename NodeList::iterator>		    NodeMap;
		typedef HashMap<KeyType, DWORD>							DelayNodeTick;

		ulong		mMemSize;			/// 当前所有对象所占的内存数 
		NodeList	mNodeList;			/// 标准节点列表
		NodeList	mRecycledNodeList;	/// 可回收节点列表
		NodeList	mDelayRecycledNodeList;	/// 延迟的可回收节点列表
		NodeMap		mNodeMap;			/// 节点映射
		ulong		mMaxPoolSize;		/// 缓冲池能够容纳的最大内存数

		DelayNodeTick mDelyNodeTick;    /// 记录延迟回收节点的起始时间

	public:
		ResPool()
		{
			close();
#ifdef RKT_WIN32
			MEMORYSTATUS ms;
			GlobalMemoryStatus(&ms);
			mMaxPoolSize = ms.dwTotalPhys/2;
			if (mMaxPoolSize > 64*1024*1024) // 默认最大100M
				mMaxPoolSize = 64*1024*1024;
#else
			mMaxPoolSize = 64*1024*1024;
#endif
		}

		~ResPool()
		{
			close();
		}

		void checkSize()
		{

			// 如果溢出，则释放可回收列表头部的节点，直到缓冲池不溢出
			if (isOverflow() && mRecycledNodeList.size() > 0)
			{
				//modified by xxh 20091104， 对处于状态stateError的东西，以前的策略是让它放在那里，以避免再次加载
				//这样有一定好处，但是有两点不足：1，没加载成功的东西，永远不能再次加载了，2，会使得mRecycledNodeList维护的
				//节点数量随游戏时间而剧增。所以作这样的修改：对于已经在mRecycledNodeList里面的处于stateError状态的节点，
				//在处理回收站时，把它清掉，这样就会有再次加载的机会，同时也不会频繁加载，回收站维护的stateError状态的节点不会
				//因为stateError而剧增。
				/*
				NodeList::iterator rlist_it = mRecycledNodeList.begin(); 
				while (rlist_it != mRecycledNodeList.end())
				{
					//Info("removeNode: delete object, id="<<(*rlist_it)->getKey()<<endl);
					if ((*rlist_it)->getState() != stateLoaded)
					{
						rlist_it++;
						continue;
					}
					mMemSize -= (*rlist_it)->getSize();
					LONGLONG key = (*rlist_it)->getKey();
					(*rlist_it)->release();
					mNodeMap.erase(key);
					mRecycledNodeList.erase(rlist_it);
					rlist_it = mRecycledNodeList.begin();
					if (!isOverflow())
						break;
				}
				*/
				NodeList::iterator rlist_it = mRecycledNodeList.begin(); 
				ResourceState _ResState;
				while( rlist_it != mRecycledNodeList.end() )
				{
					_ResState = (*rlist_it)->getState();
					if( _ResState == stateLoaded || _ResState == stateError )
					{
						mMemSize -= (*rlist_it)->getSize();
						LONGLONG key = (*rlist_it)->getKey();

						//不能在主线程中直接删除, 由资源加载线程删除
						//主线程只需做很少的线程同步，耗费时间的操作在资源加载线程间做同步
						//(*rlist_it)->release();
						(*rlist_it)->setState(stateRelease);
						if(gResourceManager)
							gResourceManager->releaseResourceNode(*rlist_it);

						mNodeMap.erase(key);
						mRecycledNodeList.erase(rlist_it);
						rlist_it = mRecycledNodeList.begin();
						if (!isOverflow())
							break;
					}
					else//此时可能在loading
					{
						++rlist_it;
						continue;
					}
				}
			}


			DWORD dwCurTick = ::GetTickCount();
			if (isOverflow() && mDelayRecycledNodeList.size() > 0)
			{
				// 延迟回收的节点 TODO
				NodeList::iterator rlist_it = mDelayRecycledNodeList.begin(); 
				ResourceState _ResState;
				while( rlist_it != mDelayRecycledNodeList.end() )
				{
					_ResState = (*rlist_it)->getState();
					if( _ResState == stateLoaded || _ResState == stateError )
					{
						mMemSize -= (*rlist_it)->getSize();
						LONGLONG key = (*rlist_it)->getKey();

						DWORD dwRemoveTick = mDelyNodeTick[key];
						if (dwCurTick - dwRemoveTick < 120000)
						{
							// 延迟回收的节点保存至少2分钟
							++rlist_it;
							continue;
						}

						//不能在主线程中直接删除, 由资源加载线程删除
						//主线程只需做很少的线程同步，耗费时间的操作在资源加载线程间做同步
						//(*rlist_it)->release();
						(*rlist_it)->setState(stateRelease);
						if(gResourceManager)
							gResourceManager->releaseResourceNode(*rlist_it);

						mNodeMap.erase(key);
						mDelayRecycledNodeList.erase(rlist_it);
						rlist_it = mDelayRecycledNodeList.begin();
						mDelyNodeTick.erase(key);
						if (!isOverflow())
							break;
					}
					else//此时可能在loading
					{
						++rlist_it;
						continue;
					}
				}
			}

		}

		inline void grow(ulong size)
		{
			mMemSize += size;

#ifdef  RKT_TEST
			if (mMemSize> mMaxPoolSize)
			{
				WarningOnce("object pool overflow! mNodeList.size()="<<mNodeList.size()
					<<", mRecycledNodeList.size()="<<mRecycledNodeList.size()<<endl);
			}
#endif


#ifdef  RKT_TEST
			//Info("mem="<<_ff("%.02f", ((float)mMemSize/1024))<<"K"<<endl);
#endif
		}

		void close()
		{
			for (NodeList::iterator it=mNodeList.begin(); it!=mNodeList.end(); ++it)
			{
				(*it)->release();						
			}

			for (NodeList::iterator it=mRecycledNodeList.begin(); it!=mRecycledNodeList.end(); ++it)
				(*it)->release();

			for (NodeList::iterator it=mDelayRecycledNodeList.begin(); it!=mDelayRecycledNodeList.end(); ++it)
				(*it)->release();

			mNodeList.clear();
			mRecycledNodeList.clear();
			mDelayRecycledNodeList.clear();
			mDelyNodeTick.clear();

			mNodeMap.clear();
			mMemSize = 0;
		}

		/// 判断缓冲池是否已经溢出
		inline bool isOverflow() const
		{
			return (mMemSize > mMaxPoolSize);
		}

		/// 设置缓冲池的大小
		inline void setMaxPoolSize(ulong dwMaxPoolSize)
		{
			// 空间变小而且小于当前池中所有缓冲对象的尺寸,删除可回收对象,从头向尾删
			if (dwMaxPoolSize < mMaxPoolSize && mMemSize > dwMaxPoolSize)
			{
				checkSize();
			}
			mMaxPoolSize = dwMaxPoolSize;
		}

		/// 添加一个新节点，注：添加新节点时不受缓冲池容量的限制
		//modified by xxh
		//增加参数bIncMemSize的目的是考虑到辅助线程也会增加内存大小，为了准确控制内存大小，所以增加了参数bIncMemSize
		inline bool addNode(PoolNodeType& node)
		{
#if  0 /*RKT_TEST*/
			if (mMemSize + node->getSize() > mMaxPoolSize)
			{
				WarningOnce("object pool overflow! mNodeList.size()="<<mNodeList.size()
					<<", mRecycledNodeList.size()="<<mRecycledNodeList.size()<<endl);
			}
#endif

			// 添加一个对象,并记录到哈希表
			node->incRef();
			mNodeList.push_back(node);
			mNodeMap[node->getKey()] = --mNodeList.end();
			//modified by xxh , 因为load的时候会增长MemSize,所以这里不需要增长。否则内存加倍。同时mMemSize是否要加锁，也是个问题
			//mMemSize += node->getSize();

			//removeNode(node->getKey());

#if  0 /*RKT_TEST*/
			Info(
				"addNode: key="<<node->getKey()
				<<",ref="<<node->getRefCount()
				<<",size="<<_ff("%.02f", ((float)node->getSize()/1024))
				<<"K,count="<<mNodeList.size()
				<<",mem="<<_ff("%.02f", ((float)mMemSize/1024))<<"K"<<endl);
#endif
			checkSize();

			return true;
		}

		/// 移除节点，如果引用计数为0，但缓冲池还有空间则将对象扔到垃圾桶，不进行真正的删除
		inline bool removeNode(KeyType idKey, bool bDelayRecycle)
		{
			NodeMap::iterator it = mNodeMap.find(idKey);
			if (it != mNodeMap.end()) // exist!
			{
				NodeList::iterator list_it = (*it).second;

				//Info("removeNode:"<<(LONGLONG)idKey<<endl);

				if((*list_it)->decRef() == 0)
				{
						//Info("扔到可回收列表的尾部:"<<(LONGLONG)idKey<<endl);
						//Info("removeNode: recycle object, id="<<(*list_it)->getKey()<<endl);

						//PoolNodeType pNode = (*list_it);
						/*if(pNode->getSize() == 0xfeeefeee)
						{
							breakable;
						}
						mNodeMap.erase(idKey);
						mNodeList.erase(list_it);
						pNode->release();
						return true;*/
						if (bDelayRecycle)
						{
							//放入延迟回收列表
							mDelayRecycledNodeList.push_back(*list_it);
							mNodeMap[idKey] = --mDelayRecycledNodeList.end();
							// 记录当前时间
							mDelyNodeTick[idKey] = ::GetTickCount();
						}
						else
						{
							//放入可回收列表
							mRecycledNodeList.push_back(*list_it);
							mNodeMap[idKey] = --mRecycledNodeList.end();
						}
						
						mNodeList.erase(list_it);
						checkSize();
				}
				else
				{
					//Info("无法扔到可回收列表的尾部:"<<(LONGLONG)idKey<<endl);
				}

				return true;
			}
			return false;
		}


		/// 查找对应键值的节点，找到则添加引用计数，如果在回收列表中，则移入标准列表中
		PoolNodeType find(KeyType idKey)
		{
			NodeMap::iterator it = mNodeMap.find(idKey);
			if (it != mNodeMap.end()) // exist!
			{
				NodeList::iterator list_it = (*it).second;

				// 如果在可回收列表中，则放到标准列表中
				if ((*list_it)->getRefCount() == 0)
				{
					//Info("find: get object from recycle bin, id="<<idKey<<endl);
					mNodeList.push_back(*list_it);			// push to back
					mNodeMap[idKey] = --mNodeList.end();	// reset pointer
					if (mDelyNodeTick.find(idKey) != mDelyNodeTick.end())
					{
						mDelayRecycledNodeList.erase(list_it);
					}
					else
					{
						mRecycledNodeList.erase(list_it);		// delete node
					}					
					list_it = --mNodeList.end();			// update pointer					
				}

				(*list_it)->incRef();

				return (*list_it);
			}

			return 0;
		}
	};
}


#endif // __RESPOOL_H__