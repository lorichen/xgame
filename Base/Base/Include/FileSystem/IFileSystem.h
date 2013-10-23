#ifndef __STREAMSYSTEM_H__
#define __STREAMSYSTEM_H__

#include "Stream.h"

namespace xs{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


/// 枚举流Callback
struct IListFileCallback;

/// 文件系统接口
struct IFileSystem
{
	/**打开文件系统或者包系统
	@param streamSysPath 文件系统路径或者包文件名
	*/
	virtual bool open(const char* streamSysPath) = 0;

	/**某个文件是否位于此文件系统下
	@param streamName 文件名
	@return 是否找到
	*/
	virtual bool find(const char* streamName) = 0;
	
	/**打开某个文件
	@param streamName 文件名
	@return 文件流，可能是磁盘文件也可能是包文件
	@remarks
	*/
	virtual Stream* get(const char* streamName) = 0;

	/**枚举此文件系统下的所有文件
	@param pCallback 回调接口，用来接收查找到的文件名
	@param p 自定义参数
	*/
	virtual void list(IListFileCallback* pCallback,void *p) = 0;

	/**关闭文件系统
	@return 是否成功
	@remarks
	*/
	virtual bool close() = 0;

	/**释放文件系统
	*/
	virtual void release() = 0;


	/*
	//获取文件系统名字，方便调试
	*/
	virtual const char * getFileSystemName(void) = 0;
};


	/** @} */
}


#endif // __STREAMSYSTEM_H__
