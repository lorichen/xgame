#ifndef __MPKMANIP_H__
#define __MPKMANIP_H__

#include "IMpkManip.h"
#include "MpkLib.h"

namespace xs{

class MpkManip : public IMpkManip
{
	struct _Data
	{
		MpkManip *pThis;
		bool bReplaceExist;
		IMpkManipCallback *pCallback;
		void *pCallbackData;
		bool compress;
		bool encrypt;
	};
	static bool dirProc(const char* path, const char* relativePath, uint userData);
	static bool fileProc(const char* path, const char* relativePath, uint userData);
public:
	/**释放自己
	@param
	@param
	@return
	@remarks
	*/
	virtual void release();

	/**打开一个包文件
	@param mpkFilename 文件名
	@param bCreateNew 是否创建新文件
	@return 是否成功打开文件
	@remarks
	*/
	virtual bool open(const char* mpkFilename,bool bCreateNew);

	/**关闭文件
	@param
	@param
	@return 是否成功
	@remarks
	*/
	virtual bool close();

	/**是否有个文件叫filename
	@param
	@param
	@return 文件是否存在
	@remarks
	*/
	virtual bool hasFile(const char* filename);

	/**打开一个文件
	@param filename 文件名
	@param
	@return 文件操作对象指针
	@remarks
	*/
	virtual IMpkFileManip* openFile(const char* filename);

	/**解压目录
	@param source 源路径 "Data\\Creatures\\char"
	@param targetDir 目标路径 "C:\\Creatures"
	@param replaceExist 是否替换已经存在的文件
	@param pCallback 回调接口，让上层有机会做点事情
	@param p 自定义数据
	@return 是否成功
	@remarks
	*/
	virtual bool extractDir(const char* source,const char* targetDir,bool replaceExist,IMpkManipCallback *pCallback,void *p);

	/**解压文件
	@param source 源路径 "Data\\Creatures\\char\\1.m2"
	@param targetFilename 目标文件名 "C:\\Creatures\\1.m2"
	@param replaceExist 是否替换已经存在的文件
	@return 是否成功
	@remarks
	*/
	virtual bool extractFile(const char* source,const char* targetFilename,bool replaceExist);

	/**删除目录
	@param dir 路径 "Data\\Creatures\\char"
	@return 是否成功
	@remarks
	*/
	virtual bool removeDir(const char* dir);

	/**删除文件
	@param filename 文件名
	@param
	@return 是否成功
	@remarks
	*/
	virtual bool removeFile(const char* filename);

	/**重命名文件,如果已经有个dest文件，那么会失败
	@param source 源文件名
	@param dest 目标文件名
	@return 是否成功
	@remarks
	*/
	virtual bool renameFile(const char* source,const char* dest);

	/**添加一个文件到包内
	@param sourceFilename 文件名 "C:\\1.txt"
	@param targetFilename 目标文件 "Data\\1.txt"
	@param replaceExist 是否替换现有文件
	@param compress 是否压缩
	@param encrypt 是否加密
	@return 是否成功
	@remarks
	*/
	virtual bool addFile(const char* sourceFilename,const char* targetFilename,bool replaceExist = true,bool compress = true,bool encrypt =false);

	/**添加一个目录到包内
	@param sourceDir 目录名 "C:\\Data"
	@param targetDir 目标目录 "Data\\char"
	@param replaceExist 是否替换现有文件
	@param compress 是否压缩
	@param encrypt 是否加密
	@param pCallback 回调，对于每个文件回调一次
	@param p 自定义回调参数
	@return 是否成功
	@remarks
	*/
	virtual bool addDir(const char* sourceDir,const char* targetDir,bool replaceExist = true,bool compress = true,bool encrypt =false,IMpkManipCallback *pCallback = 0,void *p = 0);

	/**定位到第一个文件
	@param szMask 通配符"*.txt"
	@param
	@return 文件名
	@remarks
	*/
	virtual const char* findFirstFile(const char* szMask);

	/**定位到下一个文件
	@param
	@param
	@return 文件名
	@remarks
	*/
	virtual const char* findNextFile();

	/**关闭查找文件
	@param
	@param
	@return
	@remarks
	*/
	virtual void findClose();

	/**Dump所有文件
	@param
	@param
	@return
	@remarks
	*/
	virtual void dumpListFile();

	/**取得包使用率，包里面可能存在碎片
	@param mpkFilename 包文件名
	@param
	@return 百分比，88
	@remarks
	*/
	virtual uint getUseRate(const char* mpkFilename);

	/**清除包文件碎片
	@param mpkFilename 包文件名
	@param
	@return 是否成功
	@remarks
	*/
	virtual bool cleanupFragment(const char* mpkFilename);

	/**取得列表文件名，用于打包工具
	@param
	@param
	@return 列表文件名(listfile)
	@remarks用于打包工具
	*/
	virtual const std::string& getListFileName();
	MpkManip() : m_hMpk(0),m_hFind(0){}
private:
	HANDLE m_hMpk;
	HANDLE m_hFind;
	SFILE_FIND_DATA	m_FileFindData;
};

}

#endif