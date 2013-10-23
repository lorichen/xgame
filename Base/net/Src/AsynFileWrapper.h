/*******************************************************************
** 文件名:	AsynFileWrapper.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	03/04/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __ASYN_FILE_WRAPPER_H__
#define __ASYN_FILE_WRAPPER_H__

#include "IAsynfile.h"
#include "AsynFileDevice.h"
#include "AsynIoFrame.h"

namespace xs
{

	/**
	@name : 异步文件包装类
	@brief: 职责:
			1.封装异步文件访问接口
			2.由于异步读取时没有把数据读完全也会返回(例如读100字节,可能读到50字节也会返回)
			  所以这里要包装一下,实现完整读取
	*/
	class AsynFileWrapper : public IAsynFile,public AsynIoHandler
	{
	public:
		virtual bool Open(const char * path,OPEN_MODE mode,IAsynFileHandler * handler);

		virtual unsigned __int64 Size();

		virtual bool Read(unsigned __int64 pos,unsigned long expect_bytes);

		virtual bool Write(unsigned __int64 pos,const char * data,unsigned long len);

		virtual void Release();

		virtual void HandleIoComplete( AsynIoResult * result );

		handle GetSafeHandle() const { return m_hHandle; }

		IAsynFileHandler * GetEventHandler() const { return m_pHandler; }

		AsynFileWrapper() : m_pDevice(0),m_pHandler(0),m_hHandle(0){}

		virtual ~AsynFileWrapper(){}

	protected:
		AsynFileDevice      *   m_pDevice;
		IAsynFileHandler    *   m_pHandler;
		handle                  m_hHandle;
	};

};

#endif//__ASYN_FILE_WRAPPER_H__