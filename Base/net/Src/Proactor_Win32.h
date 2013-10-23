/*******************************************************************
** 文件名:	Proactor_Win32.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/24/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  Windows下Proactor实现 - 完成端口(IOCP)	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __PROACTOR_WIN32_IOCP_H__
#define __PROACTOR_WIN32_IOCP_H__

#include "Proactor.h"
#include "singleton.h"

namespace xs{

	#ifdef WIN32

	/**
	@name : Windows下Proactor实现 - 完成端口(IOCP)
	@brief: 完成端口有两个作用:
			1.分解完成事件
			2.控制并发工作线程
	@warning: 注意: Window98 Window95不支持完成端口
	*/
	class Proactor_Win32 : public Proactor,public SingletonEx<Proactor_Win32>
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		virtual bool Create(unsigned long nConcurrentThreadsNum=0);

		virtual void Close();

		virtual bool RegisterDevice(AsynIoDevice * device);

		virtual bool HandleEvents(unsigned long wait=INFINITE);

		virtual void HandleEventsLoop();

		virtual bool PostCompletion( AsynIoResult * result);

		Proactor_Win32() : m_hCompletePort(0),m_dwConcurrentThreadsNum(0)
		{}

		virtual ~Proactor_Win32()
		{
			Close();
		}

	protected:
		HANDLE  m_hCompletePort;         // 完成端口句柄
		DWORD   m_dwConcurrentThreadsNum;// 并发工作线程序数
	};

	#endif
};

#endif//__PROACTOR_WIN32_IOCP_H__