#include "StdAfx.h"
#include "ModelManager.h"
#include "ModelCodecMz.h"
#include "ModelCodecWarcraft3.h"
#include "ModelCodecMm.h"
#include "ModelCodecTx.h"
#include "ModelCodecMx.h"

namespace xs
{
	ModelManager::ModelManager(IRenderSystem* pRenderSystem) : m_pRenderSystem(pRenderSystem),m_recycleBinSize(0)
	{
	}

	void		ModelManager::release()
	{
		releaseAll();
		delete this;
	}

	static char modelExt[][16] = 
	{
		"mz",
		"mdx",
		"mm",
		"tx",
		"mx"
	};

	char*		ModelManager::getFirstModelExt()
	{
		m_it = 0;
		return getNextModelExt();
	}

	char*		ModelManager::getNextModelExt()
	{
		if(m_it >= sizeof(modelExt) / sizeof(modelExt[0]))return 0;
		return modelExt[m_it++];
	}

	struct _Temp
	{
		std::vector<std::string> *v;
		const char* model;
	};

	bool fbProc(const char* path, const char* relativePath, uint userData)
	{
		_Temp *pT = (_Temp*)userData;
		CPath title = CPath(pT->model).getFileTitle().c_str();
		CPath fb = path;
		std::string fbname = path;
		if(StringHelper::casecmp(fb.getFileExt(),"fb"))return true;

		std::string filename = fb.getFileTitle();
		strupr((char*)filename.c_str());
		strupr((char*)title.c_str());
		const char *p = strstr(filename.c_str(),title.c_str());
		if(p != filename.c_str())return true;

		pT->v->push_back(fb.getFileName());

		return true;
	}

	IModel*		ModelManager::loadModel(const char* fileName)
	{
		ResGuard<Mutex> lock(m_mutex);

		if(names.find(fileName) != names.end()) 
		{
			IModel *pModel = names[fileName];
			items[pModel]->addRef();
			return pModel;
		}
		RecycleBin::iterator it = m_recycleBin.begin();
		RecycleBin::iterator end = m_recycleBin.end();
		while(it != end)
		{
			IModel *pModel = (*it).pModel;
			if(StringHelper::casecmp(pModel->getFileName(),fileName) == 0)
			{
				do_add(fileName,pModel,(*it).pManagedItem);
				
				//解决卡机bug
				//修正回收站长度错误
				m_recycleBinSize -= (*it).pModel->getMomorySize();
				//end

				m_recycleBin.erase(it);
				return pModel;
			}
			++it;
		}

		std::string strFileName = fileName;

		std::string strExt;

		uint pos = strFileName.find_last_of(".");
		if( pos == std::string::npos )
		{
			Error("Unable to load Model file"<<strFileName.c_str()<<"- invalid extension");
			return false;
		}

		while( pos != strFileName.length() - 1 )
			strExt += strFileName[++pos];

		xs::CStreamHelper data = xs::getFileSystem()->open(strFileName.c_str());
		if(!data)
		{
			Error("Unable to open "<<strFileName.c_str());
			return false;
		}

		if(!StringHelper::casecmp(strExt,"mz"))
		{
			ModelCodecMz *pMz = new ModelCodecMz(strFileName,m_pRenderSystem);
			if(pMz)
			{
				if(pMz->decode(data))
				{
					std::string mziFile = strFileName;
					mziFile += "i";

					CPath path = strFileName.c_str();
					std::string title = path.getFileTitle();

					std::vector<std::string> vFbs;
					if(xs::getFileSystem()->isExist(mziFile.c_str()))
					{
						//如果mzi文件存在则读取非编信息
						xs::CStreamHelper mzi = xs::getFileSystem()->open(mziFile.c_str());
						if(mzi)
						{
							uchar size;
							mzi->read(&size,sizeof(size));
							for(uint i = 0;i < size;i++)
							{
								uchar len;
								mzi->read(&len,sizeof(len));
								char str[MAX_PATH];
								mzi->read(str,len);
								str[len] = 0;
								vFbs.push_back(str);
							}
						}
					}
					else
					{
						//如果是文件模式，那么枚举一下所有的非编文件
						_Temp t;
						t.model = strFileName.c_str();
						t.v = &vFbs;

						browseDirectory(path.getParentDir().c_str(),"",0,fbProc,(uint)&t,bdm_file | bdm_dir,false);
					}
					std::vector<std::string>::iterator begin = vFbs.begin();
					std::vector<std::string>::iterator end = vFbs.end();
					while(begin != end)
					{
						std::string& fb = (*begin);
						//提取非编的非模型名称部分为动画名
						CPath pathFb = fb.c_str();
						std::string fbTitle = pathFb.getFileTitle();
						std::string animationName = fbTitle.substr(title.size(),fbTitle.size() - title.size());
						if(animationName.empty() || animationName[0] != '_')
						{
							++begin;
							continue;
						}

						std::string fbFilename = path.getParentDir();
						fbFilename += "\\";
						fbFilename += pathFb.c_str();
						CStreamHelper data = xs::getFileSystem()->open(fbFilename.c_str());
						if(!data)
						{
							++begin;
							continue;
						}
						pMz->loadFeiBian(data,animationName.c_str() + 1);
						++begin;
					}

					do_add(fileName,pMz,pMz);
					return pMz;
				}
				else
				{
					delete pMz;
				}
			}
		}

		if(!StringHelper::casecmp(strExt,"tx"))
		{
			ModelCodecTx *pTx = new ModelCodecTx(strFileName,m_pRenderSystem);
			if(pTx)
			{
				if(pTx->decode(data))
				{
					std::string txiFile = strFileName;
					txiFile += "i";

					CPath path = strFileName.c_str();
					std::string title = path.getFileTitle();

					std::vector<std::string> vFbs;
					if(xs::getFileSystem()->isExist(txiFile.c_str()))
					{
						//如果txi文件存在则读取非编信息
						xs::CStreamHelper txi = xs::getFileSystem()->open(txiFile.c_str());
						if(txi)
						{
							uchar size;
							txi->read(&size,sizeof(size));
							for(uint i = 0;i < size;i++)
							{
								uchar len;
								txi->read(&len,sizeof(len));
								char str[MAX_PATH];
								txi->read(str,len);
								str[len] = 0;
								vFbs.push_back(str);
							}
						}
					}
					else
					{
						//如果是文件模式，那么枚举一下所有的非编文件
						_Temp t;
						t.model = strFileName.c_str();
						t.v = &vFbs;

						browseDirectory(path.getParentDir().c_str(),"",0,fbProc,(uint)&t,bdm_file | bdm_dir,false);
					}
					std::vector<std::string>::iterator begin = vFbs.begin();
					std::vector<std::string>::iterator end = vFbs.end();
					while(begin != end)
					{
						std::string& fb = (*begin);
						//提取非编的非模型名称部分为动画名
						CPath pathFb = fb.c_str();
						std::string fbTitle = pathFb.getFileTitle();
						std::string animationName = fbTitle.substr(title.size(),fbTitle.size() - title.size());
						if(animationName.empty() || animationName[0] != '_')
						{
							++begin;
							continue;
						}

						std::string fbFilename = path.getParentDir();
						fbFilename += "\\";
						fbFilename += pathFb.c_str();
						CStreamHelper data = xs::getFileSystem()->open(fbFilename.c_str());
						if(!data)
						{
							++begin;
							continue;
						}
						pTx->loadFeiBian(data,animationName.c_str() + 1);
						++begin;
					}

					do_add(fileName,pTx,pTx);
					return pTx;
				}
				else
				{
					delete pTx;
				}
			}
		}

		if(!StringHelper::casecmp(strExt,"mx"))
		{
			ModelCodecMx *pMx = new ModelCodecMx(strFileName,m_pRenderSystem);
			if(pMx)
			{
				if(pMx->decode(data))
				{
					//文件没有非编信息
					do_add(fileName,pMx,pMx);
					return pMx;
				}
				else
				{
					delete pMx;
				}
			}
		}

		if(!StringHelper::casecmp(strExt,"mdx"))
		{
			ModelCodecWarcraft3 *pMm = new ModelCodecWarcraft3(strFileName,m_pRenderSystem);
			if(pMm)
			{
				if(pMm->decode(data))
				{
					do_add(fileName,pMm,pMm);
					return pMm;
				}
				else
				{
					delete pMm;
				}
			}
		}

		if(!StringHelper::casecmp(strExt,"mm"))
		{
			ModelCodecMm *pMm = new ModelCodecMm(strFileName,m_pRenderSystem);
			if(pMm)
			{
				if(pMm->decode(data))
				{
					do_add(fileName,pMm,pMm);
					return pMm;
				}
				else
				{
					delete pMm;
				}
			}
		}

		return 0;
	}

	IModel*		ModelManager::getByName(const char* fileName)
	{
		return get(fileName);
	}

	void		ModelManager::releaseModel(IModel* pModel)
	{
		del(pModel);
	}

	void		ModelManager::releaseModel(const char* fileName)
	{
		delByName(fileName);
	}

	void		ModelManager::releaseAll()
	{
		std::map<IModel*,ManagedItem*>::iterator e = items.end();
		for(std::map<IModel*,ManagedItem*>::iterator b = items.begin();b != e;++b)
		{
			delete (*b).second;
		}
		items.clear();
		names.clear();

		RecycleBin::iterator it = m_recycleBin.begin();
		RecycleBin::iterator end = m_recycleBin.end();
		while(it != end)
		{
			(*it).pModel->release();
			++it;
		}
		m_recycleBin.clear();
		m_recycleBinSize = 0;
	}

	void  ModelManager::doDelete(IModel* id)
	{
		if(!id)return;

		std::map<IModel*,ManagedItem*>::iterator it = items.find(id);
		if(it == items.end())return;

		BananaSkin bs;
		bs.pModel = id;
		bs.pManagedItem = (*it).second;

		m_recycleBin.push_back(bs);
		m_recycleBinSize += id->getMomorySize();
		//模型管理器，现在改成将显卡内存和agp都算作内存，所以内存大小要修改
#define MODEL_MANAGER_RECYCLE_BIN_SIZE  ( 16L*1024L*1024L + 16L*1024L*1024L)
		while(m_recycleBinSize > MODEL_MANAGER_RECYCLE_BIN_SIZE && !m_recycleBin.empty()) //暂时定为：内存的16m和显存的16m
		{
			BananaSkin& bs = m_recycleBin.front();
			m_recycleBinSize -= bs.pModel->getMomorySize();
			bs.pModel->release();
			m_recycleBin.pop_front();
		}
	}

	//add by yhc 2010.1.29
			//去掉重复和多余的keyframe 
			//位置
			#define FLOAT_OUT_RANGE(X,Y,R) X<Y-R||X>Y+R 
			void CompressPos(KeyFrames<Vector3> &inKeyFramesPos,KeyFrames<Vector3> &outKeyFramesPos,float fRange)
			{
				outKeyFramesPos.clearKeyFramesAndKeyFrameRanges();

				uint nKeyFrames = inKeyFramesPos.numKeyFrames();
				for( uint ii = 0; ii < nKeyFrames; ii++)
				{
					KeyFrame<Vector3> * kf =  inKeyFramesPos.getKeyFrame(ii);
					if(ii==0)
					{
						outKeyFramesPos.addKeyFrame(*kf);
						continue;
					}

					//目前我试出来的最好的方法,将x,y,z分别比较
					//还有一种是学辉说的用距离的平方来比较,我的测试是效果不明显
					if( FLOAT_OUT_RANGE( kf->v[0],outKeyFramesPos.getKeyFrame(outKeyFramesPos.numKeyFrames()-1)->v[0],fRange)
						||FLOAT_OUT_RANGE( kf->v[1],outKeyFramesPos.getKeyFrame(outKeyFramesPos.numKeyFrames()-1)->v[1],fRange)
						||FLOAT_OUT_RANGE( kf->v[2],outKeyFramesPos.getKeyFrame(outKeyFramesPos.numKeyFrames()-1)->v[2],fRange)
						)
					{
						outKeyFramesPos.addKeyFrame(*kf);
					}
				}
			}

			void CompressQua(KeyFrames<Quaternion>  &inKeyFramesQua,KeyFrames<Quaternion>  &outKeyFramesQua,float fRange)
			{
				outKeyFramesQua.clearKeyFramesAndKeyFrameRanges();

				uint nKeyFrames = inKeyFramesQua.numKeyFrames();
				for( uint ii = 0; ii < nKeyFrames; ii++)
				{
					KeyFrame<Quaternion> * kf =  inKeyFramesQua.getKeyFrame(ii);
					if(ii==0)
					{
						outKeyFramesQua.addKeyFrame(*kf);
						continue;
					}

					if( FLOAT_OUT_RANGE( kf->v.x,outKeyFramesQua.getKeyFrame(outKeyFramesQua.numKeyFrames()-1)->v.x,fRange)
						||FLOAT_OUT_RANGE( kf->v.y,outKeyFramesQua.getKeyFrame(outKeyFramesQua.numKeyFrames()-1)->v.y,fRange)
						||FLOAT_OUT_RANGE( kf->v.z,outKeyFramesQua.getKeyFrame(outKeyFramesQua.numKeyFrames()-1)->v.z,fRange)
						||FLOAT_OUT_RANGE( kf->v.w,outKeyFramesQua.getKeyFrame(outKeyFramesQua.numKeyFrames()-1)->v.w,fRange)
						)
					{
						outKeyFramesQua.addKeyFrame(*kf);
					}
				}
			}
}