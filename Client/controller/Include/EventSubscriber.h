/******************************************************************************
** 文件名:	EventSubscriber.h
** 版  权:	(C)  
** 
** 日  期:	2008/5/26   13:36
** 版  本:	1.0
** 描  述:	Event Subscriber wrapper			
** 应  用:  
******************************************************************/
#pragma once

class EventSubscriber : public IEventExecuteSink
{
	
	struct _EventInfo_
	{
		WORD			wEventId;
		unsigned char	sourceType;
		unsigned long	dwSerNo;
	};

	HashMap<std::string,_EventInfo_> m_eventMap;


	UID		m_uid;

	unsigned long m_dbid;
public:
	EventSubscriber():m_uid(0),m_dbid(0){}
	~EventSubscriber(){}

	EventSet	m_executeEventset;

	void setOwner(const LONGLONG uid);
	LONGLONG getOwner()  { return m_uid;}

	void close();

	//execute event
	void subscriberExecute(WORD wEventId,const std::string& subscriber_name,bool isSubscriberAll = false,const char* pEventMsg = 0);
	void unSubscriberExecute(WORD wEventId,bool isSubscriberAll = false);

	void subscriberExecute(WORD wEventId,unsigned char sourceType,const std::string& subscriber_name,const char* pEventMsg = 0);
	void unSubscriberExecute(WORD wEventId,unsigned char sourceType);

	
	/************************************************************************/
	/* interface function                                                   */
	/************************************************************************/
	virtual void	OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

private:
	std::string getEvent(WORD wEventId);
	
};


EventSubscriber* CreateEventSubscriber();

void DeleteEventSubscriber(EventSubscriber* pSubscriber);