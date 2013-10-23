#ifndef __ModelManager_H__
#define __ModelManager_H__

#include "RenderEngine/Manager.h"

namespace xs
{
	class ModelManager : public IModelManager,public Manager<IModel*>
	{
	public:
		ModelManager(IRenderSystem* pRenderSystem);
	public:
		/** 销毁模型管理器
		*/
		virtual void 		release() ;									

		virtual char*		getFirstModelExt();

		virtual char*		getNextModelExt();

		/** 加载模型
		@param fileName 模型文件名
		@return 模型接口指针
		*/
		virtual IModel*		loadModel(const char* fileName) ;					

		/** 从文件名获取模型接口指针
		@param fileName 模型文件名
		@return 模型接口指针
		*/
		virtual IModel*		getByName(const char* fileName) ;					

		/** 销毁模型
		@param pModel 模型接口指针
		*/
		virtual void 		releaseModel(IModel* pModel) ;				

		/** 根据名字销毁模型
		@param fileName 模型文件名
		*/
		virtual void 		releaseModel(const char* fileName) ;				

		/** 销毁所有模型
		*/
		virtual void 		releaseAll() ;

	public:

		/** 实际的删除函数
		@param id 要删除的对象
		@remarks 需要实现此函数来做真正的删除
		*/
		virtual void  doDelete(IModel* id);

	private:
		IRenderSystem*		m_pRenderSystem;

		uint				m_it;

		struct BananaSkin
		{
			IModel *pModel;
			ManagedItem *pManagedItem;
		};

		typedef std::list<BananaSkin>	RecycleBin;
		RecycleBin			m_recycleBin;
		size_t				m_recycleBinSize;
	};

}

#endif