/*******************************************************************
** 文件名:	AsynFileWrapper.cpp 
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

#include "stdafx.h"
#include "AsynFileWrapper.h"
#include "AsynFileDevice.h"
#include "SafeReleaseHelper.h"
#include "CallbackDispatcher.h"
#include "CallbackObject.h"

using namespace xs;

bool AsynFileWrapper::Open(const char * path,OPEN_MODE mode,IAsynFileHandler * handler)
{
	m_pDevice = new AsynFileDevice;
	if ( !m_pDevice->Create(path))
	{
		return false;
	}

	m_hHandle  = createHandle((ulong)this);
	m_pHandler = handler;
	return true;
}

unsigned __int64 AsynFileWrapper::Size()
{
	if ( m_pDevice )
	{
		return m_pDevice->GetSize();
	}

	return 0;
}

bool AsynFileWrapper::Read(unsigned __int64 pos,unsigned long expect_bytes)
{
	if ( m_pDevice )
	{
		m_pDevice->SeekTo(pos);
		return m_pDevice->Reader()->read(expect_bytes,this);
	}

	return false;
}

bool AsynFileWrapper::Write(unsigned __int64 pos,const char * data,unsigned long len)
{
	if ( m_pDevice )
	{
		m_pDevice->SeekTo(pos);
		return m_pDevice->Writer()->write(data,len,this);
	}

	if ( m_hHandle )
	{
		closeHandle(m_hHandle);
		m_hHandle = 0;
	}

	return false;
}

void AsynFileWrapper::Release()
{
	if ( m_pDevice )
	{
		m_pDevice->Release();
		m_pDevice = 0;
	}

	delete this;
}

void AsynFileWrapper::HandleIoComplete( AsynIoResult * result )
{
	AsynIoDevice * device = result->GetAsynDevice();
	assert(device);
	assert(m_pHandler);

	if ( result->Success() )
	{
		switch(result->GetType())
		{
		case ASYN_IO_READ:                   // Data Recieve
			{
				AsynIoBuffer * pBuffer = result->GetBufferList();
				assert(pBuffer && result->GetBufferCount()==1);
				LPVOID pData = pBuffer->buf;
				DWORD  dwDataLen = result->GetTransferredBytes();
				uint64 pos   = result->GetOffsetBytes();

				result->SetBuffer(0,0);          // Detach The Buff

				// 删除result
				device->DeleteAsynIoResult(result);
				CallbackDispatcher::getInstance().RequestCallback(new Callback_Read(this,pos,pData,dwDataLen));
			}
			break;
		case ASYN_IO_WRITE:                  // Send OK
			{
				DWORD dwError = result->GetErrorCode();
				uint64 pos    = result->GetOffsetBytes();
				DWORD dwDataLen = result->GetTransferredBytes();
				device->DeleteAsynIoResult(result);
				CallbackDispatcher::getInstance().RequestCallback(new Callback_Write(this,dwError,pos,dwDataLen));
				break;
			}
		default:
			// 删除result
			device->DeleteAsynIoResult(result);
			break;
		}
	}else
	{
		// 投递出错信息
		// 删除result

		switch(result->GetType())
		{
		case ASYN_IO_READ:                   // Data Recieve
			{
				DWORD dwError = result->GetErrorCode();
				uint64 pos    = result->GetOffsetBytes();
				DWORD dwDataLen = result->GetTransferredBytes();
				device->DeleteAsynIoResult(result);
				CallbackDispatcher::getInstance().RequestCallback(new Callback_Read_Error(this,dwError,pos,dwDataLen));
				break;
			}
		case ASYN_IO_WRITE:
			{
				DWORD dwError = result->GetErrorCode();
				uint64 pos    = result->GetOffsetBytes();
				DWORD dwDataLen = result->GetTransferredBytes();
				device->DeleteAsynIoResult(result);
				CallbackDispatcher::getInstance().RequestCallback(new Callback_Write(this,dwError,pos,dwDataLen));
				break;
			}
		default:
			device->DeleteAsynIoResult(result);
			break;
		}
	}
}

NET_API IAsynFile * CreateAsynFile()
{
	return new AsynFileWrapper;
}