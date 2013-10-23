#include "StdAfx.h"
#include "Manager.h"

namespace xs
{
	ManagedItem::ManagedItem(const std::string& n): m_strName(n),m_i32Refcount(0)
	{
	}

	ManagedItem::~ManagedItem()
	{
	}

	void  ManagedItem::addRef()
	{
		++m_i32Refcount;
	}

	bool ManagedItem::delRef()
	{
		return --m_i32Refcount == 0;
	}
}

