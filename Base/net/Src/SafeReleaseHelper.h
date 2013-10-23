/*******************************************************************
** 文件名:	SafeReleaseHelper.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	02/29/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __SAFE_RELEASE_HELPER_H__
#define __SAFE_RELEASE_HELPER_H__

#include "singleton.h"
#include "AsynIoDevice.h"
#include "AsynIoResult.h"
#include "AsynIoFrame.h"
#include "Proactor.h"
#include "net.h"

namespace xs
{
	/**
	@name :
	@brief: closesocket调用后完成端口会将所有pending状态的相关异步io操作返回
	但是由于处理完成事件的工作线程不一定和调用Close的线程是同一个
	所以这里不能直接delete pending状态的结果对象,可选的做法有
	1.Post一个消息给工作线程,让工作线程去Release AsynIODevice
	2.在result对象中设置一个标志,延迟到该操作完成时delete它,当然要确保不要出现内存泄露
	使用第一种方法较为稳妥.

	这个类的作用就是Post消息给完成端口,完成后delete指定Device
	@warning: 
	         这种做法似乎还是有问题，待完善...
	         CHEN.TAO 2008.3.5 
	*/
	class SafeReleaseHelper : public AsynIoHandler,public Singleton<SafeReleaseHelper>
	{
	public:
		virtual void HandleIoComplete( AsynIoResult * result )
		{
			AsynIoDevice * pDevice = result->GetAsynDevice();
			assert(pDevice);

			AsynIoType type = result->GetType();
			pDevice->DeleteAsynIoResult(result);

			if ( type==ASYN_IO_CLOSE )
			{
				pDevice->Release();
			}
		}

		void ReleaseAsynIoDevice(AsynIoDevice * pDevice)
		{
			AsynIoResult * result = pDevice->CreateAsynIoResult(ASYN_IO_CLOSE,this);
			GetAsynIoProactor()->PostCompletion(result);
		}

		virtual ~SafeReleaseHelper()
		{
			// 帮助释放一下通讯层
			UninitializeNetwork();
		}
	};
}

#endif//__SAFE_RELEASE_HELPER_H__