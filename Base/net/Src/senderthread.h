/*******************************************************************
** 文件名:	SenderThread.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/30/2008
** 版  本:	1.0
** 描  述:	专门用于发送数据的线程
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __SENDER_THREAD_H__
#define __SENDER_THREAD_H__

#include "AsynIoFrame.h"
#include "Handle.h"
#include "Thread.h"
#include "LockFree.h"
#include "singleton.h"
using namespace xs;

#include <list>
#include <vector>
#include <map>


namespace xs{
	/**
	@name : 发包线程
	@brief: 专门用于发送数据的线程,主要用途有两个
			1.减轻用户线程发送数据带来的压力
			2.把一个连接先后发送的多块数据批量批交给IO设备
	*/
	class SenderThread : public IRunnable,public SingletonEx<SenderThread>
	{
	public :
		SenderThread()
		{
			lSendQueueDeth = 0;
		}
	protected:
		struct Send_Request
		{
			//crr mod,定义的句柄包装,纯粹多此一举，而且其非线程安全，放弃使用
			//handle         hSecurity;  // 通过该句柄判断device是否被释放
			AsynIoDevice * pSender;  
			AsynIoHandler* pHandler;
			AsynIoBuffer   Buffer;
		};

		volatile LONG lSendQueueDeth; //crr add当前放入发送队列的消息数
		typedef LockFreeQueue<Send_Request>  SEND_LIST;
		SEND_LIST                  m_SendList;

		//在成员函数中定义map结构有些编译版本会出错，所以移到类开头
		typedef std::vector<AsynIoBuffer>	            BUFFER_ARRAY;

		struct  REQUEST_DATA
		{
			BUFFER_ARRAY  m_BufferArray;
			Send_Request  m_RequestInfo;
		};

		typedef std::map<AsynIoDevice*,REQUEST_DATA>	REQUEST_MAP;

	public:
		/**
		@name         : 请求发送一段数据
 		@brief        :
		@param pSocket: 发送数据的设备
		@param pHandler:发送结果的处理接口
		@param hSecurity:通过该句柄判断device是否被释放
		@param pBuff  : 要发送的数据
		@return       :
		*/
		//crr mod,定义的句柄包装,纯粹多此一举，而且其非线程安全，放弃使用
		//void  RequestSendData(AsynIoDevice * pSocket,AsynIoHandler * pHandler,handle hSecurity,AsynIoBuffer & pBuff)
		void  RequestSendData(AsynIoDevice * pSocket,AsynIoHandler * pHandler,AsynIoBuffer & pBuff)
		{
			Send_Request request;
			//request.hSecurity = hSecurity;
			request.pSender   = pSocket;
			request.pHandler  = pHandler;
			request.Buffer    = pBuff;
			if(!((AsynStreamSocket*)pSocket)->IncreaseSendRef())
			{
				return;
			}

			//crr假设带宽为100Mbit/s = 12500KByte/s	//登录，角色信息，团p等信息平均每条消息>50Byte
			//则累积100ms发送的消息条数为25k
			//为了避免发送不过来，强制让申请发送的源线程睡眠一下
			if (lSendQueueDeth >= 10000)
			{
				ErrorOnce("=============待发送消息累积过多..lSendQueueDeth=" << lSendQueueDeth << "\r\n");
				//Sleep(1);
			}

			InterlockedIncrement(&lSendQueueDeth);
			m_SendList.Add(request);
		}

		virtual void run()
		{
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
			while(1)
			{
				// 工作一轮就休息一下,把控制权交给其他线程
				if (0 == lSendQueueDeth)
				{
					Sleep(1);
				}
				
				REQUEST_MAP										m_RequestMap; // 每个设备要发送的数据数组,这样可以批量发送

				Send_Request request;

				// 取出所有数据,便于批量发送
				int iPopOutSize =0;
				while (m_SendList.Get(request))
				{
					InterlockedDecrement(&lSendQueueDeth);
					REQUEST_MAP::iterator it = m_RequestMap.find(request.pSender);
					if ( it==m_RequestMap.end() )
					{
						m_RequestMap[request.pSender].m_BufferArray.push_back(request.Buffer);
						m_RequestMap[request.pSender].m_RequestInfo = request;
					}else
					{
						// 有可能处理类不同,得特别注意
						REQUEST_DATA & request_data = it->second;
						if ( request_data.m_RequestInfo.pHandler != request.pHandler )
						{
							BatchSend(request_data.m_RequestInfo,request_data.m_BufferArray);
							request_data.m_RequestInfo.pHandler = request.pHandler;
							request_data.m_BufferArray.clear();
						}
						request_data.m_BufferArray.push_back(request.Buffer);
					}

					iPopOutSize++;
					if (iPopOutSize >= 2)
					{
						break;
					}
					
				}

				REQUEST_MAP::iterator it = m_RequestMap.begin();
				for ( ;it!=m_RequestMap.end();++it )
				{
					REQUEST_DATA & requestData = it->second;
					Send_Request & request     = requestData.m_RequestInfo;

					BatchSend(request,requestData.m_BufferArray);
				}
			}
		}

		void BatchSend(Send_Request & request,std::vector<AsynIoBuffer> & BufferArray)
		{
			//crr mod, 定义的句柄包装,纯粹多此一举，而且其非线程安全，放弃使用
// 			if ( !isValidHandle(request.hSecurity))
// 			{
// 				return;
// 			}

			// 批量发送数据
			bool bRetSuccess = true;
			for ( size_t i=0;i<BufferArray.size();i+=MAX_ASYN_IO_BUFF_COUNT)
			{
				DWORD count = min(MAX_ASYN_IO_BUFF_COUNT,BufferArray.size()-i);

				//发送失败
				if(bRetSuccess)
				{
					bRetSuccess = request.pSender->Writer()->writeEx(&BufferArray[i],count,request.pHandler);
				}

				((AsynStreamSocket*)request.pSender)->DecreaseSendRef(count);
			}		
		}

		virtual void release(){	delete this;}
	};
}

#endif//__SENDER_THREAD_H__