#ifndef __Manager_H__
#define __Manager_H__

#include <string>
#include <map>
/**
modified by  xxh 20091012
修改内容：Manager模版处理字符串时，对字符的大小写敏感。这样做有一个bug，比如调用函数del(pModel)，假设pModel的文件名为TEST.MZ，而此时
ManagedItem的名字为test.mz,此时在删除的时候只是删除items的内容，却没有删除names的内容，造成manager的内容的不一致。原因是我们在manager外部
认为TEST.MZ和test.mz是指同一个文件，而Manager去不认为。现在修改Manager，使得它对字符大小写不敏感。
修改方法：用wrapper类ManagerHelper_LowerStringMap取代std::map<std::string , IDTYPE>，ManagerHelper_LowerStringMap会在内部转化为小写。
这样就使得manager对字符串的大小写不敏感了。

*/

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	/** added by xxh,
	一个模版使得std::map<std::string, class T> 的关键字对大小写不敏感。
	用于使得manager对名字大小写不敏感
	*/
	template <class IDTYPE>
	class _RenderEngineExport ManagerHelper_LowerStringMap
	{
	private:
		std::map<std::string, IDTYPE> m_realMap;
	public:
		ManagerHelper_LowerStringMap() {}
		IDTYPE & operator[] ( const std::string  & key )
		{
			std::string _str(key);
			StringHelper::toLower( _str);
			return m_realMap[_str];
		}

		typename std::map<std::string , IDTYPE>::iterator end() { return m_realMap.end(); }

		typename std::map<std::string, IDTYPE>::const_iterator end() const { return m_realMap.end(); }

		typename std::map<std::string, IDTYPE>::const_iterator find( const std::string & str) const
		{
			std::string _str( str );
			StringHelper::toLower(_str );
			return m_realMap.find( _str );
		}

		typename std::map<std::string, IDTYPE>::iterator find ( const  std::string & str)
		{
			std::string _str( str );
			StringHelper::toLower(_str );
			return m_realMap.find(_str);
		}

		typename std::map<std::string, IDTYPE>::iterator erase( typename  std::map<std::string , IDTYPE>::const_iterator it)
		{
			return m_realMap.erase(it);
		}

		void clear() { m_realMap.clear(); }

	};

	/** 带引用计数的对象管理单位基类
	*/
	class _RenderEngineExport ManagedItem 
	{
	private:
		int m_i32Refcount;
	public:
		std::string m_strName;
		ManagedItem(const std::string& name);
		virtual ~ManagedItem();

		void  addRef();
		bool delRef();
	};

	/** 带引用计数的对象管理器
	*/
	template <class IDTYPE>
	class _RenderEngineExport Manager 
	{
	protected: 
		//modified by xxh 20091012 , 用ManagerHelper_LowerStringMap取代std::map<std::string,IDTYPE>,实现全部字符转化为小写。
		//std::map<std::string,IDTYPE>			names; 
		ManagerHelper_LowerStringMap<IDTYPE> names;

		std::map<IDTYPE,ManagedItem*>	items; 
		xs::Mutex	m_mutex; 

	public:
		Manager(){ }

		/** 删除一个对象
		@param id 对象的类型
		*/
		virtual void  del(IDTYPE id)
		{
			m_mutex.Lock();
			if (items.find(id) == items.end())
			{
				m_mutex.Unlock();
				return;
			}
			if (items[id]->delRef()) 
			{
				ManagedItem *i = items[id];
				std::string name = i->m_strName;
				doDelete(id);
				std::map<std::string,IDTYPE>::iterator it = names.find(name);
				if(it != names.end())names.erase(it);

				std::map<IDTYPE,ManagedItem*>::iterator it1 = items.find(id);
				if(it1 != items.end())items.erase(it1);
			}
			m_mutex.Unlock();
		}

		/** 根据名字删除对象
		@param name 对象名称
		*/
		void  delByName(const std::string& name)
		{
			m_mutex.Lock();
			if (has(name)) del(get(name)); 
			m_mutex.Unlock();
		}

		/** 实际的删除函数
		@param id 要删除的对象
		@remarks 需要实现此函数来做真正的删除
		*/
		virtual void  doDelete(IDTYPE id)
		{
		}

		/** 是否存在某个名称的对象
		@param name 名称
		@return 是否存在
		*/
		bool has(const std::string& name)
		{
			m_mutex.Lock();
			bool ok = (names.find(name) != names.end());
			m_mutex.Unlock();
			return ok;
		}

		/** 获得对象
		@param name 名称
		@return 对象
		*/
		IDTYPE get(const std::string& name)
		{
			m_mutex.Lock();
			IDTYPE t = names[name];
			m_mutex.Unlock();
			return t;
		}

	protected:
		void  do_add(const std::string& name,IDTYPE id,ManagedItem* item)
		{
			m_mutex.Lock();
			names[name] = id;
			item->addRef();
			items[id] = item;
			m_mutex.Unlock();
		}
	};
	/** @} */
}

#endif

