/**
* Created by pk 2008.01.08
*/

#include "stdafx.h"
#include "Object.h"
#include <hash_map>

#define new RKT_NEW

namespace xs {

	typedef HashMap<std::string, RuntimeClass*>	RuntimeClassMap;
	RuntimeClassMap gClassMap;

	RKT_API void regClass(RuntimeClass* newClass)
	{
		if (newClass)
			gClassMap[newClass->className] = newClass;
	}

	Object* RuntimeClass::createObject()
	{
		if (procCreateObject == NULL)
		{
			Error("Trying to create object which is not DECL_DYNCREATE or DECL_SERIAL: "<<className<<endl);
			return NULL;
		}

		Object* obj = NULL;
		obj = (*procCreateObject)();
		return obj;
	}

	/*static*/
	RuntimeClass* RKT_STDCALL RuntimeClass::fromName(const std::string& className)
	{
		RuntimeClassMap::iterator it = gClassMap.find(className);
		if (it != gClassMap.end())
			return (*it).second;
		return NULL;
	}

	/*static*/
	Object* RKT_STDCALL RuntimeClass::createObject(const std::string& className)
	{
		RuntimeClass* pClass = fromName(className);
		if (pClass == NULL)
		{
			Error("Cannot find "<<className<<" RuntimeClass.  Class not defined"<<endl);
			return NULL;
		}

		return pClass->createObject();
	}


	void RuntimeClass::store(Archive& ar) const
	{
		ushort nLen = (ushort)className.length();
		ar << nLen;
		ar.write(className.c_str(), nLen*sizeof(char));
	}

	/*static*/
	RuntimeClass* RKT_STDCALL RuntimeClass::load(Archive& ar)
	{
		ushort nLen;
		char szClassName[64];

		ar >> nLen;

		if (nLen >= _countof(szClassName) ||
			ar.read(szClassName, nLen*sizeof(char)) != nLen*sizeof(char))
		{
			return NULL;
		}
		szClassName[nLen] = '\0';

		RuntimeClass* pClass = fromName(szClassName);
		if (pClass == NULL)
			Warning("Cannot load "<<szClassName<<" from archive.  Class not defined"<<endl);

		return pClass;
	}




	const struct RuntimeClass Object::classObject = { "Object", sizeof(Object), NULL };




	Archive::Archive()
		: mMode(Archive::modeStore), mBegin(0), mCur(0), mEnd(0), m_pStoreMap(NULL)
	{

	}

	Archive::Archive(uchar* buf, size_t bytes)
		: mMode(Archive::modeLoad),	m_pLoadArray(NULL)
	{
		mBegin = mCur = buf;
		mEnd = mBegin + bytes;
	}

	Archive::~Archive()
	{
		clear();
	}

	void Archive::clear()
	{
		if (isStoring() && mBegin)
			delete[] mBegin;
		mBegin = mCur = mEnd = 0;
	}

	size_t Archive::read(void* buf, size_t bytes)
	{
		Assert(isLoading());
		if (!isLoading()) return 0;

		if (!buf || bytes == 0) return 0;
		size_t toRead = bytes;
		if (mCur + toRead > mEnd)
			toRead = mEnd - mCur;
		memcpy(buf, mCur, toRead);
		mCur += toRead;
		return toRead;
	}

	void Archive::write(const void* buf, size_t bytes)
	{
		Assert(isStoring());
		if (!isStoring()) return;

		grow(bytes);
		memcpy(mCur, buf, bytes);
		mCur += bytes;
	}

	void Archive::seek(int offset)
	{
		if (offset > 0)
		{
			if (mCur + offset > mEnd)
				mCur = mEnd;
		}
		else
		{
			if (mCur + offset < mBegin)
				mCur = mBegin;
		}
	}

	void Archive::reserve(size_t bytes)
	{ 
		if (bytes > capacity())
			grow(capacity() - bytes); 
	}

	void Archive::grow(size_t n)
	{
		Assert(isStoring());
		if (!isStoring()) return;

		if (mCur + n > mEnd)
		{
			size_t nSize = size();
			size_t nCapacity = capacity();
			size_t newCapacity = nCapacity ? nCapacity : 512;

			while (newCapacity < nSize + n)
				newCapacity += newCapacity / 2;
			newCapacity = (newCapacity + 3) & ~3;

			uchar* newBuff = new uchar[newCapacity];
			if (mBegin > 0 && nSize > 0)
				memcpy(newBuff, mBegin, nSize);
			if (mBegin)
				delete[] mBegin;

			mBegin = newBuff;
			mEnd = mBegin + newCapacity;
			mCur = mBegin + nSize;
		}		
	}

	void Archive::initContainer()
	{
		if (isStoring())
		{
			if (!m_pStoreMap)
			{
				m_pStoreMap = new StoreMap();
				(*m_pStoreMap)[NULL] = 0;
			}
		}
		else
		{
			if (!m_pLoadArray)
			{
				m_pLoadArray = new LoadArray();
				m_pLoadArray->push_back(SLoadInfo(NULL, typeUndefined));
			}
		}
	}

	Object* Archive::readObject()
	{
		Assert(isLoading());
		if (!isLoading()) return NULL;

		int objIndex = 0;
		RuntimeClass* pClassRef = readClass(&objIndex);

		// check to see if tag to already loaded object
		Object* pOb = NULL;
		if (pClassRef == NULL)
		{
			const SLoadInfo& li = m_pLoadArray->at(objIndex);
			if (typeRuntimeClass == li.type)
				return NULL;

			pOb = (Object*)li.ptr;
		}
		else
		{
			// allocate a new object based on the class just acquired
			pOb = pClassRef->createObject();
			if (pOb == NULL)
				return NULL;

			m_pLoadArray->push_back(SLoadInfo((void*)pOb, typeObject));
			pOb->serialize(*this);
		}

		return pOb;
	}

	void Archive::writeObject(const Object* obj)
	{
		Assert(isStoring());
		if (!isStoring()) return;

		if (!m_pStoreMap)
			initContainer();

		if (!obj)
			*this << (int)0;
		else
		{
			StoreMap::iterator it = m_pStoreMap->find((void*)obj);
			if (it != m_pStoreMap->end())
			{
				*this << (*it).second;
			}
			else
			{
				// write class of object first
				RuntimeClass* runtimeClass = obj->getRuntimeClass();
				writeClass(runtimeClass);

				// enter in stored object table, checking for overflow
				(*m_pStoreMap)[(void*)obj] = m_pStoreMap->size();

				// cause the object to serialize itself
				((Object*)obj)->serialize(*this);
			}
		}
	}

	RuntimeClass* Archive::readClass(int* objIndex)
	{
		Assert(isLoading());
		if (!isLoading()) return NULL;

		if (!m_pLoadArray)
			initContainer();

		int index;
		*this >> index;

		RuntimeClass* runtimeClass;
		if (index == NewClassTag)
		{
			// new object follows a new class id
			if ((runtimeClass = RuntimeClass::load(*this)) == NULL)
				return NULL;

			m_pLoadArray->push_back(SLoadInfo(runtimeClass, typeRuntimeClass));
		}
		else
		{
			if (index <= 0)
				return NULL;

			const SLoadInfo& li = m_pLoadArray->at(index);
			if (typeObject == li.type)
				return NULL;

			runtimeClass = (RuntimeClass*)li.ptr;
			Assert(runtimeClass != NULL);
		}

		if (objIndex)
			*objIndex = index;

		return runtimeClass;
	}

	void Archive::writeClass(const RuntimeClass* runtimeClass)
	{
		Assert(runtimeClass != NULL);
		Assert(isStoring());
		if (!runtimeClass || !isStoring()) return;

		if (!m_pStoreMap)
			initContainer();

		StoreMap::iterator it = m_pStoreMap->find((void*)runtimeClass);
		if (it != m_pStoreMap->end())
		{
			*this << (*it).second;
		}
		else
		{
			*this << NewClassTag;
			runtimeClass->store(*this);

			(*m_pStoreMap)[(void*)runtimeClass] = m_pStoreMap->size();
		}
	}
/*
	void Archive::serializeClass(const RuntimeClass* runtimeClass)
	{
		if (isStoring())
			writeClass(runtimeClass);
		else
			readClass();
	}*/
}