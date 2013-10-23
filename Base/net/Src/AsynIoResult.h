/*******************************************************************
** 文件名:	AsynIoResult.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/24/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __ASYN_IO_RESULT_H__
#define __ASYN_IO_RESULT_H__

#include "AsynIoFrame.h"

namespace xs {

#ifdef  WIN32

	/**
	@name : Window下的完成对象实现
	@brief: Windows下的完成对象继承一个重叠结构，用于异步IO
	*/
	class AsynIoResultImp : public AsynIoResult
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		virtual void SetHandler( AsynIoHandler * handler);

		virtual void SetEventHandle( HANDLE event );

		virtual void Complete();

		virtual void PrepareBuffer( unsigned long len );
		//crr add 为接收剩余数据准备异步IO要用的缓冲区
		virtual	void PrepareBuffer( unsigned long len, LPVOID pRecieved, DWORD dwRecievedLen, DWORD dwRestLen);
		//上一次接收到的数据包不完整，返回已接收到的数据长度
		virtual DWORD GetLastRecievedLen(void) const;

		virtual void  SetBuffer(AsynIoBuffer * pBuffList,DWORD dwBuffCount);

		virtual AsynIoBuffer * GetBufferList();

		virtual DWORD GetBufferCount() const;

		virtual DWORD GetTransferredBytes() const;

		virtual void  SetTransferredBytes( DWORD bytes);

		virtual ULONG64 GetOffsetBytes();

		virtual void  SetOffsetBytes(ULONG64 offset);

		virtual BOOL  Success() const;

		virtual DWORD GetErrorCode() const;

		virtual void  SetErrorCode( DWORD err );

		virtual AsynIoDevice * GetAsynDevice() const;

		virtual void SetAsynDevice( AsynIoDevice * device);

		virtual AsynIoType GetType() const;

		virtual void SetType( AsynIoType op_type );

		virtual bool IsSimulation() { return m_bSimulation;}

		virtual void SetSimulation(bool is_simulation) {m_bSimulation=is_simulation;}

		AsynIoResultImp(); 

		AsynIoResultImp(AsynIoType opType,AsynIoHandler * handler);

		virtual ~AsynIoResultImp();

	protected:
		//////////////////////////////////////////////////////////////////////////
		AsynIoHandler *   m_pHandler;
		AsynIoDevice  *   m_pDevice;
		AsynIoBuffer      m_BuffList[MAX_ASYN_IO_BUFF_COUNT];
		DWORD             m_dwBuffCount;
		DWORD             m_dwTransferredBytes;
		DWORD             m_dwErrorCode;
		ULONG64           m_OffsetBytes;
		AsynIoType        m_Type;
		bool              m_bSimulation;

		//上一次接收到的数据包的一部分，保存在m_BuffList[0].buf开头位置，其已接收的数据长度
		DWORD			  m_dwLastPacketRecievedLen;
	};

#endif
};

#endif//__ASYN_IO_RESULT_H__