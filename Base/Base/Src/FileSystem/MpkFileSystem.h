#ifndef __MPKSTREAMSYSTEM_H__
#define __MPKSTREAMSYSTEM_H__

#include <vector>
#include "FileStream.h"
#include "IMpkManip.h"
#include "IFileSystem.h"

namespace xs{

/// 包流系统
class  MpkFileSystem : public IFileSystem
{
public:
	MpkFileSystem();
	~MpkFileSystem();

	// IFileSystem
public:
	/**打开文件系统或者包系统
	@param streamSysPath 文件系统路径或者包文件名
	*/
	virtual bool open(const char* streamSysPath);

	/**某个文件是否位于此文件系统下
	@param streamName 文件名
	@return 是否找到
	*/
	virtual bool find(const char* streamName);

	/**打开某个文件
	@param streamName 文件名
	@param
	@return 文件流，可能是磁盘文件也可能是包文件
	@remarks
	*/
	virtual Stream* get(const char* streamName);

	/**枚举此文件系统下的所有文件
	@param pCallback 回调接口，用来接收查找到的文件名
	@param p 自定义参数
	@return
	@remarks
	*/
	virtual void list(IListFileCallback* pCallback,void *p);

	/**关闭文件系统
	@param
	@param
	@return 是否成功
	@remarks
	*/
	virtual bool close();

	/**释放文件系统
	@param
	@param
	@return
	@remarks
	*/
	virtual void release();


	/*
	//获取文件系统名字，方便调试
	*/
	virtual const char * getFileSystemName(void);

private:
	IMpkManip* m_pMpkManip;

	std::string m_strMpkFileName;

};

}


#endif // __PKGSTREAMSYSTEM_H__
