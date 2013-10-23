/*******************************************************************
** 文件名:	AsynIoResult.cpp 
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

#include "stdafx.h"
#include "AsynIoFrame.h"
#include "AsynIoResult.h"

namespace xs {

#ifdef WIN32


AsynIoResultImp::AsynIoResultImp() : m_pHandler(0),m_dwBuffCount(0),m_pDevice(0),m_bSimulation(false),
                                  m_dwTransferredBytes(0),m_dwErrorCode(ERROR_SUCCESS),m_OffsetBytes(0),m_Type(ASYN_IO_UNKOWN)
{ 
	memset((OVERLAPPED*)this,0,sizeof(OVERLAPPED));
}

AsynIoResultImp::AsynIoResultImp(AsynIoType opType,AsynIoHandler * handler): 
                                  m_pHandler(handler),m_dwBuffCount(0),m_pDevice(0),m_bSimulation(false),
                                  m_dwTransferredBytes(0),m_dwErrorCode(0),m_OffsetBytes(0),m_Type(opType),
								  m_dwLastPacketRecievedLen(0)
{
	memset((OVERLAPPED*)this,0,sizeof(OVERLAPPED));
}

AsynIoResultImp::~AsynIoResultImp()
{
	for ( DWORD i=0;i<m_dwBuffCount;++i )
	{
		DeallocateAsynIoBuffer(m_BuffList[i].buf);
	}
	
	m_dwBuffCount=0;
}


void AsynIoResultImp::SetHandler( AsynIoHandler * handler)
{
	m_pHandler = handler;
}


void AsynIoResultImp::SetEventHandle( HANDLE event )
{
	hEvent = event;
}


void AsynIoResultImp::Complete()
{
	if ( m_pHandler )
	{
		m_pHandler->HandleIoComplete(this);
	}
}


void AsynIoResultImp::PrepareBuffer( unsigned long len )
{
	for ( DWORD i=0;i<m_dwBuffCount;++i )
	{
		DeallocateAsynIoBuffer(m_BuffList[i].buf);
	}

	m_dwBuffCount=1;

    m_BuffList[0].buf = (char *)AllocateAsynIoBuffer(len);
	m_BuffList[0].len = len;
}

void AsynIoResultImp::PrepareBuffer( unsigned long len, LPVOID pRecieved, DWORD dwRecievedLen, DWORD dwRestLen)
{
	for ( DWORD i=0;i<m_dwBuffCount;++i )
	{
		DeallocateAsynIoBuffer(m_BuffList[i].buf);
	}

	m_dwBuffCount=1;

	len = max(len, dwRecievedLen + dwRestLen);
	m_BuffList[0].buf = (char *)AllocateAsynIoBuffer(len);
	m_BuffList[0].len = len;
	memcpy(m_BuffList[0].buf, pRecieved, dwRecievedLen);
	m_dwLastPacketRecievedLen = dwRecievedLen;
}

DWORD AsynIoResultImp::GetLastRecievedLen(void) const
{
	return m_dwLastPacketRecievedLen;
}

void  AsynIoResultImp::SetBuffer(AsynIoBuffer * pBuffList,DWORD dwBuffCount)
{
	assert ( dwBuffCount<=MAX_ASYN_IO_BUFF_COUNT );

	for ( DWORD i=0;i<dwBuffCount;++i )
	{
		m_BuffList[i] = pBuffList[i];
	}

	m_dwBuffCount = dwBuffCount;
}

AsynIoBuffer * AsynIoResultImp::GetBufferList()
{
	return m_dwBuffCount==0 ? 0 : m_BuffList;
}

DWORD AsynIoResultImp::GetBufferCount() const
{
	return m_dwBuffCount;
}

DWORD AsynIoResultImp::GetTransferredBytes() const
{
	return m_dwTransferredBytes;
}


void  AsynIoResultImp::SetTransferredBytes( DWORD bytes)
{
	m_dwTransferredBytes = bytes;
}


ULONG64 AsynIoResultImp::GetOffsetBytes()
{
	return m_OffsetBytes;
}


void  AsynIoResultImp::SetOffsetBytes(ULONG64 offset)
{
	OVERLAPPED::Offset = (DWORD)offset;
	OVERLAPPED::OffsetHigh = (DWORD)(offset >> 32);
	m_OffsetBytes = offset;
}


BOOL  AsynIoResultImp::Success() const
{
	return m_dwErrorCode == ERROR_SUCCESS;
}


DWORD AsynIoResultImp::GetErrorCode() const
{
	return m_dwErrorCode;
}


void  AsynIoResultImp::SetErrorCode( DWORD err )
{
	m_dwErrorCode = err;
}

AsynIoDevice * AsynIoResultImp::GetAsynDevice() const
{
	return m_pDevice;
}

void AsynIoResultImp::SetAsynDevice( AsynIoDevice * device)
{
	m_pDevice = device;
}

AsynIoType AsynIoResultImp::GetType() const
{
	return m_Type;
}

void AsynIoResultImp::SetType( AsynIoType op_type )
{
	m_Type = op_type;
}

#endif//WIN32

};