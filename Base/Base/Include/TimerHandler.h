/*******************************************************************
** 文件名:	TimerHandler.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/22/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __TIMER_HANDLER_H__
#define __TIMER_HANDLER_H__

#include "Common.h"
#include "Trace.h"

namespace xs {

	//////////////////////////////////////////////////////////////////////////
	/*********************** 定时器处理接口 *********************************/
	// 使用方法:
	//           class A : public TimerHandler
	//           {
	//                  virtual void OnTimer( unsigned long dwTimerID )
	//                  { do something .... }
	//           }
	//
	//          A a;
	//          TimeAxis.SetTimer(1000,&a);
	//          TimeAxis.KillTimer(&a);
	//////////////////////////////////////////////////////////////////////////


	// ---------------------------------------------------------------------------
	// Name	: 定时器触发后的处理接口
	// Desc : （应用层使用时尽量从TimerHandler继承）
	// ---------------------------------------------------------------------------
	struct ITimerHandler
	{
		/**
		@purpose          : 定时器触发后回调,你可以在这里编写处理代码
		@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
		@return		      : empty
		*/
		virtual void OnTimer( unsigned long dwTimerID ) = 0;

		/**
		@purpose          : 取得存放定时器内部数据的指针
		@note             : 这个函数和应用层无关,参照TimerHandler基类的实现即可
		@note             : 时间轴具体实现中需要提高定时器的插入删除效率，所以让应用层记录一个指针
		@return		      : 返回指向临时信息的指针,该指针必须初始化成0
		*/
		virtual void ** GetTimerInfoPtr() = 0;
	};

	// ---------------------------------------------------------------------------
	// Name	: 定时器处理类基类
	// Desc : 主要是封装一下GetTimerInfoPtr的实现
	// ---------------------------------------------------------------------------
	class RKT_EXPORT TimerHandler : public ITimerHandler
	{
	public:
		virtual void ** GetTimerInfoPtr() { return &_InfoPtr; }  

		TimerHandler() : _InfoPtr(0)
		{
		}

		virtual ~TimerHandler()
		{
			Assert( _InfoPtr==0 );
		}

	protected:
		void * _InfoPtr; // 指向临时数据的指针
	};

} // namespace

#endif//__TIMER_HANDLER_H__