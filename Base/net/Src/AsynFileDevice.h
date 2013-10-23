/*******************************************************************
** 文件名:	AsynFileDevice.h 
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

#ifndef __ASYN_FILE_DEVICE_H__
#define __ASYN_FILE_DEVICE_H__

#ifdef WIN32

#include "AsynIoFrame.h"
#include "Proactor.h"
#include "AsynIoResult.h"
#include "AsynIoDevice.h"
#include "AsynIoOperation.h"

#include <set>

namespace xs{

	/**
	@name : 异步文件设备
	@brief: 实现对文件的异步读写
	*/
	class AsynFileDevice : public AsynIoDevice
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		virtual HANDLE GetHandle() const 
		{ 
			return m_hHandle; 
		}

		virtual AsynIoOperation_Reader * Reader() 
		{
			return m_pReader; 
		}

		virtual AsynIoOperation_Writer * Writer() 
		{ 
			return m_pWriter; 
		}

		virtual AsynIoOperation_Acceptor * Acceptor() 
		{ 
			return 0; 
		}

		virtual AsynIoOperation_Connector * Connector() 
		{ 
			return 0; 
		}

		virtual AsynIoResult * CreateAsynIoResult(int iotype=0,AsynIoHandler * handler=0);

		virtual void DeleteAsynIoResult(AsynIoResult * result);

		virtual void Release();

		bool Create(const char * path);

		void Close();

		void SeekTo(uint64 pos) { m_Position=pos; }

		uint64 GetCurPos() { return m_Position; }

		uint64 GetSize();

		AsynFileDevice() : m_hHandle(0),m_Position(0),m_pReader(0),m_pWriter(0){}

		virtual ~AsynFileDevice() { Close();}

	protected:
		//////////////////////////////////////////////////////////////////////////
		HANDLE                      m_hHandle;   // 文件句柄
		uint64                      m_Position;  // 当前文件读写的位置

		AsynIoOperation_Reader *    m_pReader;   // 读取器
		AsynIoOperation_Writer *    m_pWriter;   // 写入器

		typedef std::set<AsynIoResult *> RESULT_LIST;
		RESULT_LIST                 m_pendingList;// 未完成的操作

		Mutex                       m_Locker;    // 线程锁
	};

	/**
	@name : 异步文件读操作
	@brief:
	*/
	class AsynFileDevice_Reader : public AsynIoOperation_Reader
	{
	public:
		virtual bool read(DWORD expect_bytes,AsynIoHandler * handler);

		AsynFileDevice_Reader(AsynIoDevice* device) : m_pDevice(device){}

	protected:
		AsynIoDevice *   m_pDevice;
	};

	/**
	@name : 异步文件写操作
	@brief:
	*/
	class AsynFileDevice_Writer : public AsynIoOperation_Writer
	{
	public:
		virtual bool write(const char * data,DWORD len,AsynIoHandler * handler);

		virtual bool writeEx( AsynIoBuffer * pBuffList,DWORD dwBuffCount,AsynIoHandler * handler);

		AsynFileDevice_Writer(AsynIoDevice * device) : m_pDevice(device){}

	protected:
		AsynIoDevice *   m_pDevice;
	};
};

#endif//WIN32
#endif//__ASYN_FILE_DEVICE_H__