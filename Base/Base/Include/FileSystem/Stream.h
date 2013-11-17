#ifndef __STREAM_H__
#define __STREAM_H__

#include <stdio.h>
#include "Common.h"
#include "Path.h"

namespace xs{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


/// 流（基类）
class RKT_EXPORT Stream
{
protected:
	CPathA	m_path;		/// 流路径

	// 构造（析构）
public:
	Stream() { m_path.isFile(true); }
	Stream(const char* path);
	virtual ~Stream();

	// 接口方法
public:
	/** 打开流(文件流默认以只读、二进制形式打开，其他流类型不关心打开模式)
	 @param mode 流打开模式
	 @return 成功返回true，否则返回false
	 @note 有关文件流打开模式的信息：<br>

	- "r"	读    ,文件不存在则失败<br>
	- "w"	写    ,文件不存在则创建,存在则清空内容<br>
	- "a"	添加  ,文件不存在则创建<br>
	- "r+"	读写  ,文件不存在则失败<br>
	- "w+"	读写  ,文件不存在则创建,存在则清空内容<br>
	- "a+"	读添加,文件不存在则创建<br>
	- "t"	文本方式<br>
	- "b"	二进制方式<br>
	 */
	virtual bool open(const char* mode = "rb") { return false; }

	/// 关闭流（跟open对应）
	virtual bool close() { return false; }


	/** 从流中读取数据（对于包文件中的文件对象拒绝所有越界的读写）
	 @param buffer 缓冲区地址
	 @param toRead 要读取的字节大小
	 @return 成功返回true，否则返回false
	 */
	virtual bool read(IN OUT void* buffer, uint toRead) { return false; }

	/** 从流中读取字符串（对于包文件中的文件对象拒绝所有越界的读写）
	@param str 缓冲区地址
	@param length 缓冲区的最大长度，实际返回的长度也用这个参数带回
	@return 成功返回true，否则返回false
	*/
	virtual bool readString(IN OUT char* str,IN OUT uint& length) { return false; }
	
	/** 向流中写入数据（对于包文件中的文件对象拒绝所有越界的读写）
	 @param buffer 缓冲区地址
	 @param toWrite 要写入的字节大小
	 @return 成功返回true，否则返回false
	 */
	virtual bool write(const void* bufer, uint toWrite)  { return false; }

	/** 文件读写位置定位
	 @param offset 偏移量
	 @param from 从那种指定的位置开始进行偏移，参考: SEEK_SET 文件头; SEEK_CUR 文件当前位置; SEEK_END 文件尾
	 @return 
	 */ 
	virtual bool seek(int offset, uint from = SEEK_SET) = 0;

	/// 文件读写位置定位到文件头
	virtual bool seekToBegin()  = 0;

	/// 文件读写位置定位到文件尾
	virtual bool seekToEnd()  = 0;

	/// 获得文件指针的位置
	virtual int getPosition() const = 0;


	/// 将文件的缓冲区内容写入磁盘(包文件不支持)
	virtual bool flush()  { return false; }


	/// 获取文件长度(文件没打开时也能获取长度),如果是包文件,则是指压缩后的长度
	virtual uint getLength()  = 0;

	/// 设置流的长度
	virtual bool setLength(uint newLen)  { return false; }

	/** 设置流路径
	 @param path 流的路径名
	 @return 成功返回true，否则返回false
	 */ 
	virtual bool setPath(const char* path);


	/// 流是否存在(注: 包流不支持,包流时可通过get(Handle, name)的返回值来判断是否存在)
	virtual bool isExist() const { return false; }

	/// 流是否处于开启状态
	virtual bool isOpen() const	{ return false; }


	/// 删除流
	virtual bool remove() { return false; }

	/** 重命名流
	 @param newName 新的流名
	 @return 成功返回true，否则返回false
	 */ 
	virtual bool rename(const char* newName) { return false; }

	/** 流存储为文件
	 @param newName 要保存的文件名
	 @return 成功返回true，否则返回false
	 */
	virtual bool save(const char* newName) { return false; }

	/// 释放流对象
	virtual void release();
};


//////////////////////////////////////////////////////////////////////////

/** 切换到文件的全路径名
 @param path 要转换的文件名（可以是当前目录的相对路径）
 @note 不检查文件或文件所在的各级目录是否存在，检查是否存在使用@c checkPath
 */
 RKT_API void toggleFullPath(CPathA& path);

/** 检查路径是否存在（包括目录路径和文件路径）
 @param path 路径名
 @param isAbsolutePath 是否是绝对路径
 @param attrib 返回文件（或目录）的属性
 @return 成功返回true，否则返回false
 */
RKT_API bool checkPath(IN const char* path, IN OUT bool& isAbsolutePath, IN OUT uint& attrib);

/** 检查路径是否是文件
 @param path 要检查的路径（如果是相对路径，会自动以exe进程所在目录为当前目录进行处理）
 @return 成功返回true，否则如果路径不存在或者是目录则返回false
 */
RKT_API bool isFile(const char* path);

/** 检查路径是否是目录
 @param path 要检查的路径（如果是相对路径，会自动以exe进程所在目录为当前目录进行处理）
 @return 成功返回true，否则如果路径不存在或者是文件则返回false
 */
RKT_API bool isDirectory(const char* path);

/** 检查指定的路径是否是绝对路径
 @param path 要检查的路径
 @return 绝对路径返回true，否则返回false
 */
RKT_API bool isAbsolutePath(const char* path);

/** 递归创建目录
 @param absolutePath 要创建的绝对路径
 @return 成功或者目录已存在返回true，否则返回false，失败后不处理可能创建了的目录
 */
RKT_API bool createDirectoryRecursive(const char* absolutePath);

/** 递归删除目录
 @param absolutePath 要删除的绝对路径
 @return 成功或者目录不存在返回true，否则返回false
 */
RKT_API bool removeDirectoryRecursive(const char* absolutePath);

/// 目录浏览mask
enum BrowseDirMask{bdm_file=0x1, bdm_dir=0x2};

/** 浏览目录
 @param absoluteDir 绝对目录路径
 @param relativeDir 相对目录，用于记录绝对目录下面所有的子目录的相对位置
 @param DIRPROC 目录回调函数
 @param FILEPROC 文件回调函数
 @param userData 用户提供的私有数据
 @param mask 用于浏览文件和目录的mask，mask＝0x1表示只浏览目录，mask=0x2表示只浏览文件，可进行or操作
 @return 成功返回true，否则返回false
 @note 如：browseDirectory("e:\\mytestdir", "", CPkgOperation::dirProc, CPkgOperation::fileProc, (uint)this, 3);
 */
RKT_API bool browseDirectory(const char* absoluteDir, const char* relativeDir, bool (*DIRPROC)(const char*, const char*, uint), 
					  bool (*FILEPROC)(const char*, const char*, uint), uint userData, uint mask = bdm_file|bdm_dir,bool recursive = true);

	/** @} */
}

#endif // __STREAM_H__
