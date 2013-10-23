/**
* Created by pk 2008.01.03
*/

#ifndef RKT_TIMER_H
#define RKT_TIMER_H

#include "Common.h"
#include "Api.h"

namespace xs {
/*
	/// 高性能时钟
	class RKT_EXPORT CTimer
	{
		int64 mBeginTime;	/// 时钟开始时间点
		int64 mEndTime;		/// 时钟结束时间点

	public:
		CTimer();

		/// 启动时钟
		void begin();

		/// 停止时钟
		void end();

		/// 返回毫秒数（整数型）
		ulong getTime() const;

		/// 返回毫秒数（Double型）
		double getTimeByDouble() const;
	};*/

	class RKT_EXPORT Timer
	{
		int64 mBeginCount;	/// 时钟开始时的计数

	public:
		Timer();

		/// 重置时钟
		void reset();

		/// 返回毫秒数（整数型）
		ulong getTime() const;

		/// 返回毫秒数（Double型）
		double getTimeByDouble() const;
	};

}

#endif // RKT_TIMER_H