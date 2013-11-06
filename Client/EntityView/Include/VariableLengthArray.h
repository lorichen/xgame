
	/*! \addtogroup SceneManager
	*  场景管理
	*  @{
	*/
#ifndef __VariableLengthArray_H__
#define __VariableLengthArray_H__

#pragma warning(disable:4200)

template<class TYPE,int nStackSize = 256>
class Stack
{
public:
	Stack(){m_nCount = 0;}
	~Stack(){close();};
	bool isEmpty() const{return m_nCount == 0;}
	bool isFull() const{return m_nCount == nStackSize;}
	int  getCount() const{return m_nCount;}
	TYPE pop()
	{
		m_nCount--;
		return m_stack[m_nCount];
	}
	bool push(TYPE node)
	{
		if(m_nCount >= nStackSize)
			return false;
		m_stack[m_nCount++] = node;
		return true;
	}
	TYPE getData(int nIndex){return m_stack[nIndex];}
	void close(){m_nCount = 0;}
private:
	int		m_nCount;
	TYPE	m_stack[nStackSize];
};

template<class TYPE,class DATA>
class VariableLengthArray
{
public:
	VariableLengthArray(int nArraySize)
	{
		wCount = 0;
		this->wArraySize = nArraySize;
	}
	~VariableLengthArray()
	{
		for(int i = 0; i < wCount; i++)
			data[i].~DATA();
		wCount = 0;
		wArraySize = 0;

	}

	void release()
	{
		this->~VariableLengthArray<TYPE,DATA>();
		char *p = (char*)this;
		delete[] p;
	}

	void init(int nArraySize = 32)
	{
		wCount = 0;
		this->wArraySize = nArraySize;
	}
	static VariableLengthArray<TYPE,DATA>* create(int nArraySize)
	{
		if(nArraySize < 0)
			return 0;
		VariableLengthArray<TYPE,DATA>* p = (VariableLengthArray<TYPE,DATA>*)new char[sizeof(VariableLengthArray<TYPE,DATA>) + sizeof(DATA) * nArraySize];
		if(p == 0)
			return 0;
		p->wCount = 0;
		p->wArraySize = nArraySize;

		return p;
	}

	inline bool isFull()const{return wCount >= wArraySize;}

	inline bool isEmpty()const{return wCount == 0;}
	inline bool isValid()const {return wArraySize >= wCount;}

	inline bool add(const DATA& data)
	{
		if(wCount >= wArraySize)
			return false;
		this->data[wCount++] = data;
		return true;
	}

	inline void _add(const DATA& data)
	{
		this->data[wCount++] = data;
	}

	bool removeAt(int index)
	{
		assert(index < (int)wCount);
		memmove(&data[index],&data[index + 1],(wCount - index - 1) * sizeof(DATA));
		wCount--;
		return true;
	}

	bool remove(const DATA& data)
	{
		for(int index = 0; index < (int)wCount; index++)
			if(this->data[index] == data)
			{
				memmove(&this->data[index],&this->data[index + 1],(wCount - index - 1) * sizeof(DATA));
				wCount--;
				return true;
			}
		return false;
	}

	bool zeroItem(const DATA& data)
	{
		for(int index = 0; index < (int)wCount; index++)
			if(this->data[index] == data)
			{
				(DWORD&)this->data[index] = 0;
				return true;
			}
		return false;
	}

	bool isFullZero()
	{
		for(int index = 0; index < (int)wCount; index++)
		{
			if((DWORD&)this->data[index] != 0)
				return false;
		}
		return index == (int)wCount;
	}

	inline DATA& operator[](int i )
	{
		assert(i < (int)wCount);
		return data[i];
	}
	
	//inline void* getBuffer(){return &PreData;}   //?????????
	inline DATA* getDataBuffer(){return data;}
	inline TYPE& getParamData(){return paramData;}
	inline int getCount()const{return wCount;}
	inline int getArraySize()const{return wArraySize;}
	int getMemSize()const{return sizeof(VariableLengthArray<TYPE,DATA>) + wCount * sizeof(DATA);}

protected:
	WORD wCount;
	WORD wArraySize;
    TYPE paramData;
	DATA data[];
};

template<class TYPE,class DATA>
class VariableLengthArrayCache
{
public:
	enum{maxArrayKind = 16};
	typedef VariableLengthArray<TYPE,DATA>		VariableLengthArrayType;
	typedef Stack<VariableLengthArrayType*,512>	VariableLengthArrayList;

	VariableLengthArrayCache()
	{
		memset(&m_list[0],0,sizeof(m_list));
		memset(&m_SizeList[0],0,sizeof(m_SizeList));
		m_nCount = 0;
	}
	~VariableLengthArrayCache(){close();}
	void close();
	bool addCacheList(int nArraySize);
	VariableLengthArrayType* allocArray(int nItemCount,VariableLengthArrayType* pOldArray = 0);
	void freeArray(VariableLengthArrayType* pArray);
	bool addData( VariableLengthArrayType** ppArray,DATA& data);
	bool removeData(VariableLengthArrayType** ppArray,DATA& data);
	bool zeroItem(VariableLengthArrayType** ppArray,DATA& data);

protected:
	int			m_nCount;
	VariableLengthArrayList*	m_list[maxArrayKind];
	int			m_SizeList[maxArrayKind];
};

template<class TYPE,class DATA>
void VariableLengthArrayCache<TYPE,DATA>::close()
{
	for(int i = 0; i < m_nCount; i++)
	{
		if(m_list[i])
		{
			int nCount = m_list[i]->getCount();
			for(int j = 0; j < nCount; j++)
			{
				m_list[i]->getData(j)->release();
			}
			delete m_list[i];

			m_list[i] = 0;
		}
	}
	m_nCount = 0;
}

template<class TYPE,class DATA>
bool VariableLengthArrayCache<TYPE,DATA>::addCacheList(int nSize)
{
	if(m_nCount >= maxArrayKind)
		return false;
	VariableLengthArrayList* pList = new VariableLengthArrayList();
	if(pList == 0)
		return false;
	m_SizeList[m_nCount] = nSize;
	m_list[m_nCount++] = pList;
	return true;	
};

template<class TYPE,class DATA>
bool VariableLengthArrayCache<TYPE,DATA>::addData(VariableLengthArrayType** ppArray,DATA& data)
{
	assert(ppArray != 0);
	VariableLengthArrayType* pArr;
	if(*ppArray != 0)
	{
		if((*ppArray)->add(data))
			return true;
		if(0 == (pArr = allocArray((*ppArray)->getCount() + 1,*ppArray)))
			return false;
	}
	else
	{
		if(0 == (pArr = allocArray(m_SizeList[0])))
			return false;
	}
	pArr->_add(data);
	*ppArray = pArr;
	return true;
}

template<class TYPE,class DATA>
bool VariableLengthArrayCache<TYPE,DATA>::removeData(VariableLengthArrayType** ppArray,DATA& data)
{
	assert(ppArray != 0);
	VariableLengthArrayType* pArr = *ppArray;
	if(pArr == 0)
		return 0;
	bool retv = pArr->remove(data);
	if(pArr->isEmpty())
	{
		freeArray(pArr);
		*ppArray = 0;
	}
	return retv;
}

template<class TYPE,class DATA>
bool VariableLengthArrayCache<TYPE,DATA>::zeroItem(VariableLengthArrayType** ppArray,DATA& data)
{
	assert(ppArray != 0);
	VariableLengthArrayType* pArr = *ppArray;
	if(pArr == 0)
		return 0;
	bool retv = pArr->zeroItem(data);
	if(pArr->isEmpty())
	{
		freeArray(pArr);
		*ppArray = 0;
	}
	return retv;
}


template<class TYPE,class DATA>
VariableLengthArray<TYPE,DATA>* VariableLengthArrayCache<TYPE,DATA>::allocArray(int nItemCount,VariableLengthArrayType* pOldArray)
{
	int i = 0;
	for(; m_SizeList[i] < nItemCount && i < m_nCount;i++);
	if(i >= m_nCount)
		return 0;
	VariableLengthArrayType* pArray;
	if(!m_list[i]->isEmpty())
	{
		pArray = m_list[i]->pop();
		pArray->init(m_SizeList[i]);
	}
	else if(0 == (pArray = VariableLengthArrayType::create(m_SizeList[i])))
		return 0;

	if(pOldArray)
	{
		int nCount = pOldArray->getCount();
		for(int j = 0; j < nCount; j++)
			pArray->_add((*pOldArray)[j]);
		freeArray(pOldArray);
	}
	return pArray;
}

template<class TYPE,class DATA>
void VariableLengthArrayCache<TYPE,DATA>::freeArray(VariableLengthArrayType* pArray)
{
	if(pArray == 0)
		return;
	int nSize = pArray->getArraySize();
	int i = 0;
	for(; m_SizeList[i] != nSize && i < m_nCount;i++);
	assert(i < m_nCount);

	if(!m_list[i]->push(pArray))
	{
		// delete会内存泄露 [5/5/2011 zgz]
		//delete pArray;
		safeRelease(pArray);
	}
		
}

#endif

	/** @} */