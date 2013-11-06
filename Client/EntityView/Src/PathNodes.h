//==========================================================================
/**
* @file	  : PathNodes.h
* @author : 
* created : 2008-1-31   16:16
* purpose : 管理路径节点
*/
//==========================================================================

#ifndef __PATHNODES_H__
#define __PATHNODES_H__

/// 专用的路径节点模板，不属于通用模板，给移动组件专用
template <class T>
class PathNodes
{
	T*		_nodes;		/// 节点数组
	size_t	_current;	/// 当前节点位置
	size_t	_count;		/// 节点数目
	size_t	_capacity;	/// 最大节点容量

public:
	PathNodes() 
		: _nodes(0), _count(0), _capacity(0), _current(0)
	{}

	~PathNodes()
	{ safeDeleteArray(_nodes); }

	void clear()	{ _count = _current = 0; }

	/// 预置空间
	void reserve(size_t count)
	{
		grow(count - _count);
	}

	/// 加入一个节点在后面
	void add(const T& node)
	{
		grow(1);
		_nodes[_count++] = node;
	}

	/// 添加一个节点数组
	void add(const T* nodes, size_t count)
	{
		if (nodes && count > 0)
		{
			grow(count);
			memcpy(_nodes+_count, nodes, count*sizeof(T));
			_count += count;
		}
	}

	const T& front() const	{ return _nodes[0]; }
	const T& cur() const	{ return _nodes[_current]; }
	const T& next() const	{ return _nodes[_current+1]; }
	const T& back() const	{ return _nodes[_count-1]; }

	void operator++()		{ ++_current; }
	bool isLast() const		{ return _current + 1 == _count; }
	bool empty() const		{ return size() == 0; }
	void trimRight(size_t pos)	{ _nodes[0] = _nodes[_current]; _count = 1;_current=0; }

	size_t size() const		{ return _count; }
	size_t capacity() const { return _capacity; }
	size_t current() const	{ return _current; }

private:
	inline void grow(size_t count)
	{
		if (_count + count > _capacity)
		{
			_capacity = _count + count;
			T* new_nodes = new T[_capacity];
			if (_count > 0)
				memcpy(new_nodes, _nodes, _count * sizeof(T));
			if (_nodes) delete[] _nodes;
			_nodes = new_nodes;
		}
	}
};

#endif // __PATHNODES_H__