#ifndef __Animation_H__
#define __Animation_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	/** 动画定义，ID，开始时间结束时间，是否循环
	*/
	class _RenderEngineExport Animation
	{
	public:
		inline const std::string& getName()
		{
			return m_strName;
		}
		inline uint	getTimeStart()
		{
			return m_ui32TimeStart;
		}
		inline uint	getTimeEnd()
		{
			return m_ui32TimeEnd;
		}
		inline bool isLoop()
		{
			return m_bLoop;
		}
		inline void setRangeIndex(int rangeIndex)
		{
			m_rangeIndex = rangeIndex;
		}
		inline int getRangeIndex()
		{
			return m_rangeIndex;
		}
		inline void setTimeStart(uint time)
		{
			m_ui32TimeStart = time;
		}
		inline void setTimeEnd(uint time)
		{
			m_ui32TimeEnd = time;
		}
		const Animation & operator = ( const Animation & ani)
		{
			m_ui32TimeStart = ani.m_ui32TimeStart;
			m_ui32TimeEnd = ani.m_ui32TimeEnd;
			m_bLoop = ani.m_bLoop;
			m_strName = ani.m_strName;
			m_rangeIndex = ani.m_rangeIndex;
			return *this;
		}
	private:
		uint	m_ui32TimeStart;
		uint	m_ui32TimeEnd;
		bool	m_bLoop;
		std::string	m_strName;
		int		m_rangeIndex;
	public:
		Animation(uint start,uint end,bool bLoop,const char* animation) : m_ui32TimeStart(start),m_ui32TimeEnd(end),m_bLoop(bLoop),m_strName(animation),m_rangeIndex(-1)
		{
		}
	};
	/** @} */
}

#endif