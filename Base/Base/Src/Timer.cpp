/**
* Created by pk 2008.01.03
*/

#include "stdafx.h"
#include "Timer.h"

#define new RKT_NEW

namespace xs {

	extern double count2Time(int64 deltaCount);
/*
	CTimer::CTimer() : mBeginTime(0), mEndTime(0)
	{
	}

	void CTimer::begin()
	{ 
		mBeginTime = getPerformanceCount(); 
	}

	void CTimer::end()
	{ 
		mEndTime = getPerformanceCount(); 
	}

	ulong CTimer::getTime() const
	{ 
		return (ulong)count2Time(mEndTime - mBeginTime); 
	}

	double CTimer::getTimeByDouble() const
	{ 
		return count2Time(mEndTime - mBeginTime); 
	}*/

	Timer::Timer()
	{
		reset();
	}

	void Timer::reset()
	{
		mBeginCount = getPerformanceCount();
	}

	ulong Timer::getTime() const
	{ 
		return (ulong)count2Time(getPerformanceCount() - mBeginCount); 
	}

	double Timer::getTimeByDouble() const
	{ 
		return count2Time(getPerformanceCount() - mBeginCount); 
	}

}