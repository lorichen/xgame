//==========================================================================
/**
* @file	  : MemStat.cpp
* @author : 
* created : 2008-5-23   18:59
*/
//==========================================================================

#include "stdafx.h"
#include "MemAlloc.h"
#include "path.h"

#define new RKT_NEW

namespace xs {


	void MemAllocator::log(ostrbuf& osb)
	{
		const char* msg = osb.c_str();
		OutputDebugStringA(msg);
#ifdef MEMORY_DEBUG
		if ((mMemoryDebugFlag & MDF_REPORT_TO_FILE) && mFileHandle)
			fputs(msg, mFileHandle);
#endif
		osb.clear();
	}

	const char* safeGetFile(const char* f)
	{
		if (f)
		{
			try
			{
				//if (::IsBadStringPtrA(f, 32))
				//	return "(null)";
				strlen(f);
			}
			catch (...)
			{
				return "(null)";
			}
		}
		return f;
	}

	static const char* insertCommas(size_t value)
	{
		static char str[30];

		memset(str, 0, sizeof(str));
		sprintf(str, "%d", value);
		size_t len = strlen(str);

		if (len > 3) {
			memmove(&str[len - 3], &str[len - 4], 4);
			str[len - 3] = ',';
			len++;
		}
		if (len > 7) {
			memmove(&str[len - 7], &str[len - 8], 8);
			str[len - 7] = ',';
			len++;
		}
		if (len > 11) {
			memmove(&str[len - 11], &str[len - 12], 12);
			str[len - 11] = ',';
		}
		return str;
	}

	static const char* memorySizeString(size_t size, bool lengthenString  = false)
	{
	//crr mod 2010-06-30，直接使str成为static，然后返回即可，免得stl使用operator new
	//	static std::vector<std::string> v;
	static	char str[64];
		if (lengthenString) {
			if (size > (1024*1024))	sprintf(str, "%10s", insertCommas(size));
			else if (size > 1024)	sprintf(str, "%10s", insertCommas(size));
			else				    sprintf(str, "%10s", insertCommas(size));
		}
		else {
			if (size > (1024*1024))	sprintf(str, "%d", (size));
			else if (size > 1024)	sprintf(str, "%d", (size));
			else				    sprintf(str, "%d", (size));
		}
	//	v.push_back(str);
	//	return v[v.size() - 1].c_str();
		return str;
	}

	void MemAllocator::reportMemoryLeak()
	{
#ifdef MEMORY_DEBUG
		MultiThread::scopelock lock(mMutex);
		if (mMemoryDebugFlag & MDF_REPORT_TO_FILE)
		{
			std::string filename = getWorkDir();
			filename += "\\MemoryDump.log";
			mFileHandle = fopen(filename.c_str(), "wt");
		}

		ulong leakCount = 0;
		size_t leakSize = 0;

		ostrbuf osb;

		osb<<"//////////////////////////////////////////////////////////////////////////"<<endl<<"Dump memory leak..."<<endl;
		log(osb);


		// 报告大内存泄露
		for (LM_Node* node=mLMHead.next; node!=&mLMHead; node=node->next)
		{
			//try{
			osb<<safeGetFile(node->file)<<"("<<node->line<<"): "<<memorySizeString(node->data.size)<<endl;
			log(osb);
			//}catch(...){}

			leakCount++;
			leakSize += node->data.size;
		}

		// 报告小内存泄露
		for (int i=0; i<POOL_SIZE; i++)
		{
			ulong blockSize = getSM_NodeSize(MEM_ALIGN * (i+1));	// 每个块的大小
			SmallBlock* smallBlock = mSBHead[i];
			while (smallBlock)
			{
				uchar* data = (uchar*)smallBlock->data(); // 定位到数据区
				for (int j=0; j<NODE_GROW; j++)
				{
					SM_Node* node = (SM_Node*)data;
					if (node->state == 1)
					{
						osb<<safeGetFile(node->file)<<"("<<node->line<<"): "<<memorySizeString(node->data.size)<<endl;
						log(osb);

						leakCount++;
						leakSize += node->data.size;
					}

					data += blockSize;
				}

				smallBlock = smallBlock->prev;
			}
		}

		if (leakCount > 0)
		{
			osb<<"Leak count: "<<leakCount<<endl;
			log(osb);
		}
		if (leakSize > 0)
		{
			osb<<"Leak size: "<<memorySizeString(leakSize)<<endl;
			log(osb);
		}

		osb<<endl;
		log(osb);

		if ((mMemoryDebugFlag & MDF_REPORT_TO_FILE) && mFileHandle)
			fclose(mFileHandle);
#endif
	}

#ifdef MEMORY_DEBUG
	/// 小内存枚举器
	class EnumSmallMemory
	{
		SmallBlock** mSBHead;
		int mPoolIndex;
		SmallBlock* mSmallBlock;
		int mIndex;
		ulong mBlockSize;

		uchar* mNode;
	public:
		EnumSmallMemory() {}
		EnumSmallMemory(const EnumSmallMemory& rhs)
		{
			mSBHead = rhs.mSBHead;
			mPoolIndex = rhs.mPoolIndex;
			mSmallBlock = rhs.mSmallBlock;
			mIndex = rhs.mIndex;
			mBlockSize = rhs.mBlockSize;
			mNode = rhs.mNode;
		}
		void init(SmallBlock** sbHead, int poolIndex, int sb, int index)
		{
			mSBHead = sbHead;
			mPoolIndex = poolIndex;
			mSmallBlock = mSBHead[mPoolIndex];
			while (sb)
			{
				sb--;
				mSmallBlock = mSmallBlock->prev;
			}
			mIndex = index;

			mBlockSize = getSM_NodeSize(MEM_ALIGN * (mPoolIndex+1));
			mNode = 0;
		}

		SM_Node* getNext()
		{
			if (!mNode)
			{
				if (NULL == mSmallBlock) //从来没使用过内存情况
				{
					return NULL;
				}
				mNode = (uchar*)mSmallBlock->data() + mIndex * mBlockSize;
			}
			else
			{
				mIndex++;
				if (mIndex >= NODE_GROW)
				{
					mIndex = 0;
					mSmallBlock = mSmallBlock->prev;
					//if (!mSmallBlock) //当下一个pool没有被使用时，必须一直找到下个使用的pool! crr mod, 2010-07-01
					while(!mSmallBlock)
					{
						mPoolIndex++;
						if (mPoolIndex >= POOL_SIZE)
							return NULL;
						mSmallBlock = mSBHead[mPoolIndex];

						mBlockSize = getSM_NodeSize(MEM_ALIGN * (mPoolIndex+1));
					}
					mNode = (uchar*)mSmallBlock->data();
				}
				else
					mNode += mBlockSize;
			}

			return (SM_Node*)mNode;
		}
	};

	/// 大内存枚举器
	class EnumLargeMemory
	{
		LM_Node*	mLMHead;
		LM_Node*	mNode;
	public:
		EnumLargeMemory() {}
		EnumLargeMemory(const EnumLargeMemory& rhs)
		{
			mLMHead = rhs.mLMHead;
			mNode = rhs.mNode;
		}
		void init(LM_Node* head)
		{
			mLMHead = head;
			mNode = head;
		}
		LM_Node* getNext()
		{
			mNode = mNode->next;
			if (mNode == mLMHead)
				return NULL;
			return mNode;
		}
	};
#endif

	void MemAllocator::reportMemoryStatistics()
	{
#ifdef MEMORY_DEBUG
		MultiThread::scopelock lock(mMutex);
		if (mMemoryDebugFlag & MDF_REPORT_TO_FILE)
		{
			std::string filename = getWorkDir();
			filename += "\\MemoryStat.csv";
			mFileHandle = fopen(filename.c_str(), "wt");
		}

		ostrbuf osb;

		osb<<"//////////////////////////////////////////////////////////////////////////"<<endl<<"Memory statistics..."<<endl;
		log(osb);

		osb<<"分配器共分配内存: "<<memorySizeString(mTotalAllocSize)<<endl;
		osb<<"大内存: "<<memorySizeString(mTotalLargeMemorySize)<<endl;
		osb<<"小内存请求次数: "<<mSmallMemoryRequestCount<<" 对应的内存和: "<<memorySizeString(mSmallMemoryRequestSize)<<endl;
		osb<<"使用中的小内存: "<<memorySizeString(mTotalSmallMemorySize)<<endl;
		log(osb);

		// 统计小内存
		osb<<endl<<"小内存统计："<<endl;
		ulong uninitAllNodeCount=0, usingAllNodeCount=0, freeAllNodeCount=0;
		ulong smallMemoryNodeCountRequest=0,smallMemoryNodeCountRelease=0,smallMemoryNodeCountPeak=0;
		ulong allSmallBlockCount = 0;
		for (int i=0; i<POOL_SIZE; i++)
		{
			ulong smallBlockCount = 0;
			ulong uninitNodeCount=0, usingNodeCount=0, freeNodeCount=0;

			ulong blockSize = getSM_NodeSize(MEM_ALIGN * (i+1));	// 每个块的大小
			SmallBlock* smallBlock = mSBHead[i];
			while (smallBlock)
			{
				smallBlockCount++;
				uchar* data = (uchar*)smallBlock->data(); // 定位到数据区
				for (int j=0; j<NODE_GROW; j++)
				{
					SM_Node* node = (SM_Node*)data;

					if (node->state == 0)
						uninitNodeCount++;
					else if (node->state == 1)
						usingNodeCount++;
					else if (node->state == 2)
						freeNodeCount++;

					data += blockSize;
				}

				smallBlock = smallBlock->prev;
			}

			osb<<"小内存["<<_fi("%3d", (MEM_ALIGN * (i+1)))<<"]["<<_fi("%5d*",smallBlockCount)<<NODE_GROW<<"]："
				<<",未用["<<_fi("%2d",uninitNodeCount)<<"]"
				<<",使用["<<_fi("%6d",usingNodeCount)<<"]"
				<<",空闲["<<_fi("%6d",freeNodeCount)<<"]"
				<<",请求["<<_fi("%6d",mSmallMemoryNodeCountRequest[i])<<"]"
				<<",释放["<<_fi("%6d",mSmallMemoryNodeCountRelease[i])<<"]"
				<<",峰值["<<_fi("%6d",mSmallMemoryNodeCountPeak[i])<<"]"<<endl;
			log(osb);

			uninitAllNodeCount += uninitNodeCount;
			usingAllNodeCount += usingNodeCount;
			freeAllNodeCount += freeNodeCount;
			smallMemoryNodeCountRequest += mSmallMemoryNodeCountRequest[i];
			smallMemoryNodeCountRelease += mSmallMemoryNodeCountRelease[i];
			smallMemoryNodeCountPeak += mSmallMemoryNodeCountPeak[i];
			allSmallBlockCount += smallBlockCount;
		}

		osb<<"所有小内存["<<_fi("%5d*",allSmallBlockCount)<<NODE_GROW<<"]："
			<<",未用["<<_fi("%2d",uninitAllNodeCount)<<"]"
			<<",使用["<<_fi("%6d",usingAllNodeCount)<<"]"
			<<",空闲["<<_fi("%6d",freeAllNodeCount)<<"]"
			<<",请求["<<_fi("%6d",smallMemoryNodeCountRequest)<<"]"
			<<",释放["<<_fi("%6d",smallMemoryNodeCountRelease)<<"]"
			<<",峰值["<<_fi("%6d",smallMemoryNodeCountPeak)<<"]"<<endl;
		log(osb);


		// 统计大内存
		osb<<endl<<"大内存统计："<<endl;
		osb<<"请求次数"<<mLargeMemoryNodeCountRequest<<"\t释放次数"<<mLargeMemoryNodeCountRelease<<"\t峰值"<<mLargeMemoryNodeCountPeak<<endl;
		osb<<"请求大小"<<memorySizeString(mLargeMemoryNodeSizeRequest)<<"\t释放大小"<<memorySizeString(mLargeMemoryNodeSizeRelease)<<"\t峰值"<<memorySizeString(mLargeMemoryNodeSizePeak)<<endl;
		log(osb);

		// 总结
		osb<<"最大的单次内存请求尺寸："<<memorySizeString(mMaxMemoryAllocSize)<<endl;
		log(osb);

		osb<<endl;
		log(osb);


		//////////////////////////////////////////////////////////////////////////
		// 统计内存使用情况
		const char* max_count_file = 0;
		int max_count_line = 0;
		ulong max_count = 0;

		const char* max_size_file = 0;
		int max_size_line = 0;
		size_t max_size = 0;


		osb<<endl<<"小内存使用情况统计(文件行号),个数,总内存,平均"<<endl;
		log(osb);
		{
			EnumSmallMemory esm;
			esm.init(mSBHead, 0, 0, 0);
			SM_Node* node = esm.getNext();
			while (node)
			{
				if (node->state == 1 && node->mask == '<<<<') // 找到一个新的
				{
					ulong allocCount = 0;
					size_t allocSize = 0;

					allocCount++;
					allocSize += node->data.size;

					// 找相同的
					EnumSmallMemory esm1(esm);
					SM_Node* node1 = esm1.getNext();
					while (node1)
					{
						if (node1->state == 1 && node1->mask == '<<<<' && node->file == node1->file && node->line == node1->line) // 找到一个相同的
						{
							node1->mask = '1111';
							allocCount++;
							allocSize += node1->data.size;
						}
						node1 = esm1.getNext();
					}

					if (allocCount > max_count)
					{
						max_count = allocCount;
						max_count_file = node->file;
						max_count_line = node->line;
					}

					if (allocSize > max_size)
					{
						max_size = allocSize;
						max_size_file = node->file;
						max_size_line = node->line;
					}

					osb<<safeGetFile(node->file)<<"("<<node->line<<"):,"<<allocCount<<","<<memorySizeString(allocSize)<<","<<_ff("%.2f", ((float)allocSize)/allocCount)<<endl;
					log(osb);
				}
				node = esm.getNext();
			}

			// mask改回
			esm.init(mSBHead, 0, 0, 0);
			node = esm.getNext();
			while (node)
			{
				if (node->mask == '1111')
					node->mask = '<<<<';
				node = esm.getNext();
			}
		}

		osb<<endl<<"Max count:"<<safeGetFile(max_count_file)<<"("<<max_count_line<<"):,count: "<<max_count<<endl;
		osb<<"Max size:"<<safeGetFile(max_size_file)<<"("<<max_size_line<<"):,size: "<<memorySizeString(max_size)<<endl;
		log(osb);


		// 大内存使用情况
		osb<<endl<<"大内存使用情况统计(文件行号),个数,总内存,平均"<<endl;
		log(osb);
		{
			EnumLargeMemory elm;
			elm.init(&mLMHead);
			LM_Node* node = elm.getNext();
			while (node)
			{
				if (node->mask == '>>>>') // 找到一个新的
				{
					ulong allocCount = 0;
					size_t allocSize = 0;

					allocCount++;
					allocSize += node->data.size;

					// 找相同的
					EnumLargeMemory elm1(elm);
					LM_Node* node1 = elm1.getNext();
					while (node1)
					{
						if (node->mask == '>>>>' && node->file == node1->file && node->line == node1->line) // 找到一个相同的
						{
							node1->mask = '2222';
							allocCount++;
							allocSize += node1->data.size;
						}
						node1 = elm1.getNext();
					}

					if (allocCount > max_count)
					{
						max_count = allocCount;
						max_count_file = node->file;
						max_count_line = node->line;
					}

					if (allocSize > max_size)
					{
						max_size = allocSize;
						max_size_file = node->file;
						max_size_line = node->line;
					}

					osb<<safeGetFile(node->file)<<"("<<node->line<<"):,"<<allocCount<<","<<memorySizeString(allocSize)<<","<<_ff("%.2f", ((float)allocSize)/allocCount)<<endl;
					log(osb);
				}
				node = elm.getNext();
			}

			// mask改回
			elm.init(&mLMHead);
			node = elm.getNext();
			while (node)
			{
				node->mask = '>>>>';
				node = elm.getNext();
			}
		}

		osb<<endl<<"Max count:"<<safeGetFile(max_count_file)<<"("<<max_count_line<<"):,count: "<<max_count<<endl;
		osb<<"Max size:"<<safeGetFile(max_size_file)<<"("<<max_size_line<<"):,size: "<<memorySizeString(max_size)<<endl;
		log(osb);


		if ((mMemoryDebugFlag & MDF_REPORT_TO_FILE) && mFileHandle)
			fclose(mFileHandle);
#endif
	}


} // namespace