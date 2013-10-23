#ifndef __VirtualFileSystem_H__
#define __VirtualFileSystem_H__

#include <vector>
#include "Thread.h"

namespace xs{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


	struct IListFileCallback;
	class Stream;

	/**文件系统
	@param
	@param
	@return
	@remarks
	*/
	class RKT_EXPORT FileSystem
	{
		typedef std::vector<IFileSystem*>		StreamSystemVector;
	public:

		/**添加查找路径
		@param pPath 路径
		@return 文件系统
		@remarks
		*/
		IFileSystem*		addFindPath(const char* pPath);

		/**打开一个文件
		@param pFileName 文件名
		@param mode 打开方式
		@return 流指针
		@remarks
		*/
		Stream*	open(const char* pFileName,const char* mode = "rb");

		/**通过一个文件系统打开一个文件
		@param pStreamSystem 文件系统
		@param pFileName 文件名
		@param mode 打开方式
		@return 流指针
		@remarks
		*/
		Stream*	open(IFileSystem* pStreamSystem,const char* pFileName,const char* mode = "rb");

		/**是否能找到某个文件
		@param pFileName 文件名
		@return 文件是否存在于当前查找路径
		@remarks
		*/
		bool		isExist(const char* pFileName);

		/**列举当前查找路径下的所有文件
		@param pCallback 回调
		@param p 自定义参数
		@return
		@remarks
		*/
		void		list(IListFileCallback *pCallback,void *p);
	private:
		StreamSystemVector		m_vStreamSystem;
		Mutex	m_mutex;
	public:
		FileSystem(){}
		~FileSystem();
	};

		RKT_API FileSystem*	getFileSystem();
		RKT_API IFileSystem*	openFileSystem(const char* szPath);

	class RKT_EXPORT CStreamHelper
	{
	public:
		CStreamHelper();
		CStreamHelper(Stream*);
		~CStreamHelper();
		Stream* operator->();
		operator Stream*();
		bool isNull();
		const CStreamHelper& operator=(Stream* pStream);
		void close();
		//operator bool();
	private:
		Stream*	m_pStream;
	};


	/** @} */
}

#endif