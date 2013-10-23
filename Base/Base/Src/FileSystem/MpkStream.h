#ifndef __MpkStream_H__
#define __MpkStream_H__

#include "Stream.h"

namespace xs{

struct IMpkFileManip;

class RKT_EXPORT MpkStream : public Stream
{
public:
	MpkStream(IMpkFileManip *pFileManip);
public:
	virtual bool open(const char* mode = "rb"){return true;}
	virtual bool close(){return true;}
	virtual bool read(IN OUT void* buffer, uint toRead);
	virtual bool readString(IN OUT char* str,IN OUT uint& length);
	virtual bool seek(int offset, uint from = SEEK_SET);
	virtual bool seekToBegin();
	virtual bool seekToEnd();
	virtual int getPosition() const;
	virtual uint getLength() const;
	virtual void release();
private:
	IMpkFileManip*	m_pFileManip;
};

}

#endif
