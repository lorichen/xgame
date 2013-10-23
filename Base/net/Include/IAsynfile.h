/*******************************************************************
** 文件名:	IAsynFile.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	03/04/2008
** 版  本:	1.0
** 描  述:	异步文件访问接口
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __ASYN_FILE_H__
#define __ASYN_FILE_H__

//////////////////////////////////////////////////////////////////////////
// 导出宏定义
#ifndef NET_API
#	ifdef NET_EXPORTS
#		define NET_API		extern "C" __declspec(dllexport)
#	else
#		define NET_API		extern "C" __declspec(dllimport)
#	endif
#endif//NET_API

//////////////////////////////////////////////////////////////////////////

namespace xs
{
	/// 异步文件IO处理接口
	struct IAsynFileHandler;

	enum OPEN_MODE
	{
		READ  = 0    ,  // 读文件
		WRITE        ,  // 写文件
		OPEN         ,  // 打开文件,如果不存在就创建
		ONLY_OPEN    ,  // 仅仅打开文件,如果不存在不要创建
		ALWAYS_CREATE,  // 每次都重新创建
	};

	/**
	@name : 异步文件接口
	@brief: 封装对文件的异步读写
	*/
	struct IAsynFile
	{
		/**
		@name               : 打开文件 
		@brief              :
		@param      path    : 文件路径
		@param      mode    : 打开方式
		@param      handler : 结果处理接口
		@return             : 如果打开成功返回true,否则返回false,GetLastError可查询到出错代码
		*/
		virtual bool Open(const char * path,OPEN_MODE mode,IAsynFileHandler * handler) = 0;

		/**
		@name		        : 获取文件大小 
		@brief              :  
		@return             : 返回文件大小
		*/
		virtual unsigned __int64 Size() = 0;

		/**
		@name               : 从文件中读取数据
		@brief              : 读取的结果通过回调IAsynFileHandler通知
		@param     pos      : 开始位置,要读取的数据在文件中的偏移
		@param expect_bytes : 期望读取的数据长度
		@return             : 如果读取失败则返回false,成功开始读取则返回true
		*/
		virtual bool Read(unsigned __int64 pos,unsigned long expect_bytes) = 0;

		/**
		@name               : 写入数据到文件中
		@brief              : 读取的结果通过回调IAsynFileHandler通知
		@param     pos      : 开始位置,要写入的数据在文件中的偏移
		@param     data     : 要写入的数据内容
		@param     len      : 要写入的数据长度
		@return             : 如果写入失败则返回false,成功开始写则返回true
		*/
		virtual bool Write(unsigned __int64 pos,const char * data,unsigned long len) = 0;

		/**
		@name         : 销毁对象
		@brief        : 同时会关闭文件
		*/
		virtual void Release() = 0;
	};

	/**
	@name : 异步文件读写处理接口
	@brief: 处理文件读写成功后的事件
	*/
	struct IAsynFileHandler
	{
		/**
		@name               : 读取数据成功
		@brief              :
		@param     pos      : 开始位置,要读取的数据在文件中的偏移
		@param     data     : 数据内容
		@param     len      : 数据长度
		*/
		virtual void HandleReadSuccess(unsigned __int64 pos,const char * data,unsigned long len) = 0;

		/**
		@name               : 写入数据成功
		@brief              :
		@param     pos      : 开始位置,要写入的数据在文件中的偏移
		@param     len      : 要写入的数据长度
		*/
		virtual void HandleWriteSuccess(unsigned __int64 pos,unsigned long len) = 0;

		/**
		@name               : 读取数据成功
		@brief              :
		@param     err      : 错误代码
		@param     pos      : 开始位置,要读取的数据在文件中的偏移
		@param expect_bytes : 期望读取的数据长度
		*/
		virtual void HandleReadFailed(unsigned long err,unsigned __int64 pos,unsigned long expect_bytes) = 0;

		/**
		@name               : 写入数据成功
		@brief              :
		@param     err      : 错误代码
		@param     pos      : 开始位置,要写入的数据在文件中的偏移
		@param     data     : 要写入的数据内容
		@param     len      : 要写入的数据长度
		*/
		virtual void HandleWriteFailed(unsigned long err,unsigned __int64 pos,unsigned long len) = 0;
	};

	/**
	@name         : 创建异步文件对象
	@brief        :
	@param path   : 文件路径
	@param mode   : 打开方式
	@return       : 如果打开失败返回0,否则返回对象指针
	*/
	NET_API IAsynFile * CreateAsynFile();
};

#endif//__ASYN_FILE_H__