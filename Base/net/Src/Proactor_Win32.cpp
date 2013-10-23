/*******************************************************************
** 文件名:	Proactor_Win32.cpp 
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

#include "stdafx.h"
#include <assert.h>

#include "Proactor_Win32.h"
#include "AsynIoDevice.h"
#include "AsynIoResult.h"

namespace xs{

bool Proactor_Win32::Create(unsigned long nConcurrentThreadsNum)
{
	m_dwConcurrentThreadsNum = nConcurrentThreadsNum;
	m_hCompletePort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,0,0,m_dwConcurrentThreadsNum);
	return m_hCompletePort != 0;
}

void Proactor_Win32::Close()
{
	if ( m_hCompletePort )
	{
		::CloseHandle(m_hCompletePort);
		m_hCompletePort = 0;
	}
}

bool Proactor_Win32::RegisterDevice(AsynIoDevice * device)
{
	assert( device );
	assert( m_hCompletePort );

	HANDLE completeHandle = ::CreateIoCompletionPort(device->GetHandle(),m_hCompletePort,(ULONG_PTR)device,m_dwConcurrentThreadsNum);
	return completeHandle==m_hCompletePort;
}

bool Proactor_Win32::HandleEvents(unsigned long wait)
{
	DWORD dwTransferredBytes = 0;
	ULONG_PTR completeKey  = 0;
	LPOVERLAPPED overLapped = 0;

	BOOL result = ::GetQueuedCompletionStatus(m_hCompletePort,&dwTransferredBytes,&completeKey,&overLapped,wait);

	// 操作成功
	if ( result==TRUE )
	{
		assert(overLapped);
		
		AsynIoResult * io_result = (AsynIoResult *)overLapped;

		if ( !io_result->IsSimulation())
		{
			io_result->SetTransferredBytes(dwTransferredBytes);
			io_result->SetErrorCode(ERROR_SUCCESS); 
		}

		io_result->Complete();
		return true;
	}else
	{
		int error_code = ::GetLastError();

		// 操作完成，但是出现了错误
		if ( overLapped )
		{
			AsynIoResult * io_result = (AsynIoResult *)overLapped;

			io_result->SetTransferredBytes(dwTransferredBytes);
			io_result->SetErrorCode(error_code);
			io_result->Complete();
			return true;

		}else
		{
			switch (error_code)
			{
			case WAIT_TIMEOUT:
				return true;

			case ERROR_SUCCESS:
				// Calling GetQueuedCompletionStatus with timeout value 0
				// returns FALSE with extended errno "ERROR_SUCCESS" errno =
				// ETIME; ?? I don't know if this has to be done !!
				return true;
			default:
				return false;
			}
		}
	}

	return false;
}

void Proactor_Win32::HandleEventsLoop()
{
	while(HandleEvents(INFINITE))
	{
	}
}

bool Proactor_Win32::PostCompletion( AsynIoResult * result)
{
	result->SetSimulation(true);
	return ::PostQueuedCompletionStatus(m_hCompletePort,0,0,result) == TRUE;
}

};