#ifndef __KeyFrames_H__
#define __KeyFrames_H__

#include "Interpolate.h"

namespace xs
{
	struct AnimationTime
	{
		AnimationTime()
		{
			start = 0;
			end = 0;
			current = 0;
		}

		uint start;
		uint end;
		uint current;
	};

	template<class T> struct KeyFrame
	{
		KeyFrame() : time(0)
		{
			time = 0;
		}

		KeyFrame(uint time,const T& v)
		{
			this->time = time;
			this->v = v;
		}

		KeyFrame(uint time,const T& v,const T& in,const T& out)
		{
			this->time = time;
			this->v = v;
			this->in = in;
			this->out = out;
		}

		bool operator==(uint time)
		{
			return this->time == time;
		}

		uint time;
		T v;
		T in;
		T out;
	};

	typedef std::pair<int,int>		KeyFrameRange;
	typedef std::pair<uint,uint>	KeyFrameTimeRange;

	template<class T> class KeyFrames
	{
	private:
		typedef std::vector<KeyFrame<T> >	KeyFrameList;
		typedef std::vector<KeyFrameRange>	KeyFrameRangeList;

		KeyFrameList			m_KeyFrames;
		KeyFrameRangeList		m_KeyFrameRanges;
		InterpolationType		m_InterpolationType;
		typename KeyFrameList::iterator	m_it;

		/// Static
		bool					m_bStatic;
		T						m_StaticData;

		/// GlobalSequence
		int						m_GlobalSequenceId;

	public:
		void setInterpolationType(InterpolationType interpolationType)
		{
			m_InterpolationType = interpolationType;
		}

		InterpolationType getInterpolationType()
		{
			return m_InterpolationType;
		}

		int addKeyFrameRange(const KeyFrameRange& keyFrameRange)
		{
			size_t size = m_KeyFrameRanges.size();
			m_KeyFrameRanges.push_back(keyFrameRange);
			return size;
		}

		int addKeyFrameRange(const KeyFrameTimeRange& keyFrameTimeRange)
		{
			int first = -1;
			int second = -1;

			size_t size = m_KeyFrames.size();
			for(size_t i = 0;i < size;i++)
			{
				KeyFrame<T>& keyFrame = m_KeyFrames[i];
				if(keyFrame.time == keyFrameTimeRange.first)
					first = i;
				if(keyFrame.time == keyFrameTimeRange.second)
					second = i;

				if(first != -1 && second != -1)break;
			}

			KeyFrameRange keyFrameRange;
			keyFrameRange.first = first;
			keyFrameRange.second = second;

			return addKeyFrameRange(keyFrameRange);
		}

		void setStaticData(const T& data)
		{
			m_StaticData = data;
			m_bStatic = true;
		}

		void enableStaticData( bool enable)//谢学辉修改，慎用
		{
			m_bStatic = enable;
		}

		bool isEnableStaticData()//谢学辉修改，慎用
		{
			return m_bStatic;
		}

		T getStaticData() //谢学辉修改，慎用
		{
			return m_StaticData;
		}

		size_t numKeyFrameRanges() //谢学辉修改，慎用
		{
			return m_KeyFrameRanges.size();
		}

		void clearKeyFramesAndKeyFrameRanges()//谢学辉修改，慎用
		{
			m_KeyFrames.clear();
			m_KeyFrameRanges.clear();
			m_bStatic = true;
		}

		KeyFrameRange * getKeyFrameRange( size_t index )//谢学辉修改
		{
			return &m_KeyFrameRanges[index];
		}

		void addKeyFrame(const KeyFrame<T>& keyFrame)
		{
			m_KeyFrames.push_back(keyFrame);
			m_bStatic = false;
		}

		size_t numKeyFrames()
		{
			return m_KeyFrames.size();
		}



		T getFrame(const AnimationTime *pTime)
		{
			if(m_bStatic)return m_StaticData;

			size_t size = m_KeyFrames.size();
			uint lastTime = m_KeyFrames[size - 1].time;
			uint time = pTime->current;
			//if(lastTime)time %= lastTime;

			//时间在第一帧之前
			if(time < m_KeyFrames[0].time)
				return m_StaticData;

			//在最后一帧之后
			if(time >= m_KeyFrames[size - 1].time)return m_KeyFrames[size - 1].v;

			KeyFrame<T> *pKeyFrame1 = 0;
			KeyFrame<T> *pKeyFrame2 = 0;

			KeyFrameRange range;
			range.first = 0;
			range.second = size - 1;

			return getFrame(range,pTime);
		}

		T getFrame(int keyFrameRangeIndex,const AnimationTime *pTime)
		{
			if(m_bStatic)return m_StaticData;

			if(keyFrameRangeIndex == -1)return getFrame(pTime);
			KeyFrameRange range = m_KeyFrameRanges[keyFrameRangeIndex];

			return getFrame(range,pTime);
		}

		KeyFrame<T>* getKeyFrame(size_t index)
		{
			return &m_KeyFrames[index];
		}

		void setGlobalSequenceId(int globalSequeneId)
		{
			m_GlobalSequenceId = globalSequeneId;
		}

		int getGlobalSequenceId()
		{
			return m_GlobalSequenceId;
		}

		KeyFrames<T>& operator=(const KeyFrames<T>& keyFrames)
		{
			m_KeyFrames = keyFrames.m_KeyFrames;
			m_KeyFrameRanges = keyFrames.m_KeyFrameRanges;
			m_InterpolationType = keyFrames.m_InterpolationType;
			m_bStatic = keyFrames.m_bStatic;
			m_StaticData = keyFrames.m_StaticData;
			m_GlobalSequenceId = keyFrames.m_GlobalSequenceId;

			return *this;
		}

		size_t getMemorySize()
		{
			return sizeof(*this) + m_KeyFrames.size() * sizeof(KeyFrame<T>) + m_KeyFrameRanges.size() * sizeof(KeyFrameRange);
		}

	private:
		T interpolateHermite(float r,KeyFrame<T> *pKeyFrame1,KeyFrame<T> *pKeyFrame2)
		{
			T t = xs::interpolateHermite<T>(r,pKeyFrame1->v,pKeyFrame2->v,pKeyFrame2->in,pKeyFrame1->out);
			return t;
		}

		T interpolateBezier(float r,KeyFrame<T> *pKeyFrame1,KeyFrame<T> *pKeyFrame2)
		{
			T t = xs::interpolateBezier<T>(r,pKeyFrame1->v,pKeyFrame2->v,pKeyFrame2->in,pKeyFrame1->out);
			return t;
		}

		T interpolate(float r,KeyFrame<T> *pKeyFrame1,KeyFrame<T> *pKeyFrame2)
		{
			T t = xs::interpolate(r,pKeyFrame1->v,pKeyFrame2->v);
			return t;
		}



		T getFrame(KeyFrameRange range,const AnimationTime *pTime)
		{
			uint time = pTime->current % m_KeyFrames[m_KeyFrames.size() - 1].time;

			while(range.first < range.second - 1)
			{
				int middle = ( (range.first + range.second) >> 1);//谢学辉加括号，阐明优先级别: +  >  >>
				if(time >= m_KeyFrames[range.first].time && time <= m_KeyFrames[middle].time)
				{
					range.second = middle;
				}
				else
				{
					range.first = middle;
				}
			}

			KeyFrame<T> *pKeyFrame1 = &m_KeyFrames[range.first];
			KeyFrame<T> *pKeyFrame2 = &m_KeyFrames[range.second];

			if(pKeyFrame1->time < pTime->start)
			{
				if(pKeyFrame2->time > pTime->end)
				{
					return m_StaticData;
				}
				else
				{
					return pKeyFrame2->v;
				}
			}

			if(pKeyFrame2->time > pTime->end)
			{
				return pKeyFrame1->v;
			}

			if(pKeyFrame1->time == time)return pKeyFrame1->v;
			if(pKeyFrame2->time == time)return pKeyFrame2->v;

			if(m_InterpolationType == INTERPOLATION_NONE)return pKeyFrame1->v;

			float r = (time - pKeyFrame1->time) / (float)(pKeyFrame2->time - pKeyFrame1->time);

			T t;

			switch(m_InterpolationType)
			{
			case INTERPOLATION_LINEAR:
				//modified by xxh
				//线性插值在用到4元数时不平滑，因此，将4元数的线性插值改成slerp
				//t = interpolate(r,pKeyFrame1->v,pKeyFrame2->v);
				t = interpolate(r, pKeyFrame1, pKeyFrame2);
				break;
			case INTERPOLATION_HERMITE:
				//t = interpolate(r,pKeyFrame1->v,pKeyFrame2->v);
				t = interpolateHermite(r,pKeyFrame1,pKeyFrame2);
				break;
			case INTERPOLATION_BEZIER:
				//t = interpolate(r,pKeyFrame1->v,pKeyFrame2->v);
				t = interpolateBezier(r,pKeyFrame1,pKeyFrame2);
				break;
			}

			return t;
		}
	public:
		KeyFrames() : m_InterpolationType(INTERPOLATION_NONE),m_bStatic(true),m_GlobalSequenceId(-1)
		{
		}
	};

	//added by xxh 20091021
	template<>
	Quaternion KeyFrames<Quaternion>::interpolate(float r, KeyFrame<Quaternion> * pKeyFrame1, KeyFrame<Quaternion> * pKeyFrame2 )
	{
		//这种slerp要保证旋转的角度在180度之间，否则旋转出错。

		//0,不插值
		//if( r<0.5) return pKeyFrame1->v;
		//else return pKeyFrame2->v;

		//1,默认的slerp

		//return Quaternion::Slerp( r, pKeyFrame1->v, pKeyFrame2->v, true);

		 //2,新的slerp	

		Quaternion q = pKeyFrame2->v;
		float fCos = pKeyFrame1->v.Dot(q);
		 if( fCos < 0.0f )//如果小于0，取4D短弧度
		 {
			fCos = -fCos;
			q = -q;
		 }

		 if( fCos > 0.999f)//过于接近，线性插值 
		 {
			Quaternion t_l((1-r)*pKeyFrame1->v + r*q);
			t_l.normalize();
			return t_l;
		 }

		 //球面线性插值
		 float fSin = Math::Sqrt( (float)( 1 - fCos*fCos) );
		Radian fAngle = Math::ATan2(fSin, fCos);
		float fInvSin = 1.0 / Math::Sin(fAngle);
		float fCoeff0 = Math::Sin( (Radian) ((1.0-r)*fAngle) ) * fInvSin;
		float fCoeff1 = Math::Sin(  (Radian)(r * fAngle) ) * fInvSin;
		Quaternion t_s( fCoeff0 * pKeyFrame1->v + fCoeff1 * q );
		t_s.normalize();
		return t_s;
	}

	template<>
	Quaternion KeyFrames<Quaternion>::interpolateHermite(float r,KeyFrame<Quaternion> *pKeyFrame1,KeyFrame<Quaternion> *pKeyFrame2)
	{
		Quaternion t = xs::interpolateHermite<Quaternion>(r,pKeyFrame1->v,pKeyFrame1->in,pKeyFrame2->v,pKeyFrame2->out);
		return t;
	}

	template<>
	Quaternion KeyFrames<Quaternion>::interpolateBezier(float r,KeyFrame<Quaternion> *pKeyFrame1,KeyFrame<Quaternion> *pKeyFrame2)
	{
		Quaternion t = xs::interpolateBezier<Quaternion>(r,pKeyFrame1->v,pKeyFrame1->in,pKeyFrame2->v,pKeyFrame2->out);
		return t;
	}
}

#endif