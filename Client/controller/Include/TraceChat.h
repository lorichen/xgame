/******************************************************************
** �ļ���:	TraceChat.h
** ��  Ȩ:	(C)  
** 
** ��  ��:	2008-3-21
** ��  ��:	1.0
** ��  ��:	Trace to chat window			
** Ӧ  ��:  
******************************************************************/
#pragma  once

class TraceChat : public xs::TraceListener
{
public:
	/************************************************************************/
	/* constructor and deconstructor                                        */
	/************************************************************************/
	TraceChat();
	~TraceChat();

public:
	/************************************************************************/
	/* interface functions                                                   */
	/************************************************************************/
	virtual void onTrace(const char* msg, const char* time, TraceLevel level);
};