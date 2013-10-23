
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/
#ifndef __SharedPtr_H__
#define __SharedPtr_H__

#include <assert.h>
/** Reference-counted shared pointer, used for objects where implicit destruction is 
required. 
@remarks
This is a standard shared pointer implementation which uses a reference 
count to work out when to delete the object. RE does not use this class
very often, because it is usually more efficient to make the destruction
of objects more intentional (in blocks, say). However in some cases you
really cannot tell how many people are using an object, and this approach is
worthwhile (e.g. ControllerValue)
*/
template<class T> class SharedPtr {
protected:
	T* pRep;
	unsigned int useCount;
public:
	/** Constructor, does not initialise the SharedPtr.
	@remarks
	<b>Dangerous!</b> You have to call bind() before using the SharedPtr.
	*/
	SharedPtr() : pRep(0), useCount(0) {}
	explicit SharedPtr(T* rep) : pRep(rep), useCount(1) 
	{
	}
	SharedPtr(const SharedPtr& r) 
	{
		pRep = r.pRep;
		useCount = r.useCount + 1; 
	}
	const SharedPtr& operator=(const SharedPtr& r) 
	{
		if (pRep == r.pRep)
			return *this;
		if(useCount)release();
		// lock & copy other mutex pointer
		pRep = r.pRep;
		useCount = useCount + 1;
		return *this;
	}
	virtual ~SharedPtr() 
	{
		release();
	}


	inline T& operator*() const { assert(pRep); return *pRep; }
	inline T* operator->() const { assert(pRep); return pRep; }
	inline T* get() const { return pRep; }

	/** Binds rep to the SharedPtr.
	@remarks
	Assumes that the SharedPtr is uninitialised!
	*/
	void bind(T* rep) 
	{
		assert(!pRep);
		useCount = 1;
		pRep = rep;
	}

	inline bool unique() const { return useCount == 1; }
	inline unsigned int getUseCount() const { return useCount; }

	inline T* getPointer() const { return pRep; }

	inline bool isNull(void) const { return pRep == 0; }

	inline void setNull(void) 
	{ 
		if (pRep)
		{
			// can't scope lock mutex before release incase deleted
			release();
			pRep = 0;
			useCount = 0;
		}
	}

protected:

	inline void release(void) 
	{
		bool destroyThis = false;
		if (--useCount == 0) 
		{
			destroyThis = true;
		}
		if (destroyThis)
			destroy();
	}

	virtual void destroy(void)
	{
		// IF YOU GET A CRASH HERE, YOU FORGOT TO FREE UP POINTERS
		// BEFORE SHUTTING RE DOWN
		// Use setNull() before shutdown or make sure your pointer goes
		// out of scope before RE shuts down to avoid this.
		delete pRep;
	}
};

template<class T, class U> inline bool operator==(SharedPtr<T> const& a, SharedPtr<U> const& b)
{
	return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(SharedPtr<T> const& a, SharedPtr<U> const& b)
{
	return a.get() != b.get();
}

#endif

	/** @} */