/******************************************************************************
** 文件名:	TimerWrapper.h
** 版  权:	(C)  
** 
** 日  期:	2008/5/26   15:57
** 版  本:	1.0
** 描  述:	geb			
** 应  用:  
******************************************************************/
#pragma once


unsigned long setTimer(const std::string& subscriber_name, ulong timerId, ulong interval);
void killTimer(ulong handle1);