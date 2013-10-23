#include "StdAfx.h"
#include "BoneAnimationSetManager.h"

namespace xs
{
	BoneAnimationSetManager::BoneAnimationSetManager()
	{
		m_recycleBin.clear();
		m_recycleBinSize = 0;	
	}
	BoneAnimationSetManager::~BoneAnimationSetManager()
	{
		std::map<BoneAnimationSet *, ManagedItem*>::iterator e = items.end();
		std::map<BoneAnimationSet*, ManagedItem*>::iterator b = items.begin();
		for( ; b!=e; ++b)
		{
			delete (*b).second;
		}
		items.clear();
		names.clear();

		RecycleBin::iterator itb = m_recycleBin.begin();
		RecycleBin::iterator ite = m_recycleBin.end();
		while( itb!=ite)
		{
			itb->pAni->release();
			++itb;
		}

		m_recycleBin.clear();
		m_recycleBinSize = 0;

	}

	void  BoneAnimationSetManager::doDelete(BoneAnimationSet* id)
	{
		if( !id) return;
		
		std::map<BoneAnimationSet *, ManagedItem*>::iterator it = items.find(id);
		if( it == items.end() ) return;

		BananaSkin bs;
		bs.pAni = id;
		bs.pManagedItem = it->second;

		m_recycleBin.push_back(bs);
		m_recycleBinSize += id->getMemorySize();

#define BONEANIMATION_MANAGER_BIN_SIZE ( 16L*1024L*1024L) //这个还需要确定，有6个角色的骨骼动画
		while( m_recycleBinSize > BONEANIMATION_MANAGER_BIN_SIZE && !m_recycleBin.empty())
		{
			BananaSkin & bs = m_recycleBin.front();
			m_recycleBinSize -= bs.pAni->getMemorySize();
			bs.pAni->release();
			m_recycleBin.pop_front();
		}
	}

	void BoneAnimationSetManager::releaseBoneAnimationSet(  BoneAnimationSet * pAni)
	{
		if( pAni) del(pAni);
		return;
	}

	void BoneAnimationSetManager::releaseBoneAnimationSet( const char * fileName)
	{
		delByName(fileName);
	}

	BoneAnimationSet * BoneAnimationSetManager::getByName(const char* fileName)
	{
		return get(fileName);
	}

	BoneAnimationSet * BoneAnimationSetManager::loadBoneAnimationSet( const char * fileName)
	{
		//模型管理器里面查找
		ResGuard<Mutex> lock(m_mutex);
		if( names.find(fileName) != names.end())
		{
			BoneAnimationSet * pAni = names[fileName];
			items[pAni]->addRef();
			return pAni;
		}

		//回收站查找
		RecycleBin::iterator itb = m_recycleBin.begin();
		RecycleBin::iterator ite = m_recycleBin.end();
		BoneAnimationSet * pBoneAni = 0;
		while( itb != ite)
		{
			pBoneAni = itb->pAni;
			if( StringHelper::casecmp(pBoneAni->getFileName(),fileName) == 0 )
			{
				do_add(fileName,pBoneAni,itb->pManagedItem);
				m_recycleBinSize -= pBoneAni->getMemorySize();
				m_recycleBin.erase(itb);
				return pBoneAni;
			}
			++itb;
		}
		

		//从文件构造模型
		std::string strFileName = fileName;
		std::string strExt;
		uint pos = strFileName.find_last_of(".");
		if( pos == std::string::npos )
		{
			Error("Unable to load Model file"<<strFileName.c_str()<<"- invalid extension");
			return 0;
		}
		while( pos != strFileName.length() - 1 )
			strExt += strFileName[++pos];
		if( StringHelper::casecmp( strExt, std::string("dz")) != 0 )
			return 0;

		xs::CStreamHelper data = xs::getFileSystem()->open(fileName);
		if( !data)
		{
			Error("Unable to open "<<strFileName.c_str());
			return 0;
		}

		BoneAnimationSet * pAniFromFile = new BoneAnimationSet(strFileName);
		if( pAniFromFile  )
		{
			if( pAniFromFile->decode(data) )
			{
				do_add(fileName,pAniFromFile,pAniFromFile);
				return pAniFromFile;
			
			}
			else
			{
				delete pAniFromFile;
			}
		}

		return 0;
	}

}