#ifndef __MPKFILEMANIP_H__
#define __MPKFILEMANIP_H__

#include "IMpkManip.h"
#include "MpkLib.h"

namespace xs{

class MpkFileManip : public IMpkFileManip
{
public:
	/**释放自己
	@param
	@param
	@return
	@remarks
	*/
	virtual void release();

	/**读取数据
	@param pBuffer 数据缓冲区
	@param size 要读取的数据长度
	@return 是否读取成功
	@remarks
	*/
	virtual bool read(void *pBuffer,uint size);

	/**定位文件位置
	@param length 偏移量
	@param from 从哪里开始,SEEK_SET等
	@return 是否成功
	@remarks
	*/
	virtual bool seek(uint length,uint from);

	/**获得当前文件指针
	*/
	virtual uint getPos();

	/**文件大小
	@param
	@param
	@return 文件大小
	@remarks
	*/
	virtual uint size();

	/**获得原始文件大小
	@param
	@param
	@return 原始文件大小
	@remarks
	*/
	virtual uint getOriginSize();

	/**获得压缩后的文件大小
	@param
	@param
	@return 压缩后的文件大小
	@remarks
	*/
	virtual uint getCompressedSize();

	/**是否加密
	@param
	@param
	@return 是否加密
	@remarks
	*/
	virtual bool isEncrypted();
private:
	HANDLE m_hFile;
public:
	MpkFileManip(void* hFile);
};

}

#endif