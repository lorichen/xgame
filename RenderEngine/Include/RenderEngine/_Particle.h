#ifndef __MmParticle_H__
#define __MmParticle_H__

namespace xs
{

	/** 粒子类
	*/
	struct _RenderEngineExport MmParticle
	{
		Vector3 m_vPos;		//位置
		Vector3 m_vSpeed;	//速度
		Vector3 m_vDown;	//方向
		Vector3	m_vOrigin;	//初始位置

		float	m_fSize;	//大小
		float	m_fLife;	//当前生命
		float	m_fMaxLife;	//生命周期
		int	m_i32Tile;	//处于粒子系统阵列中的位置
		ColorValue m_color;	//颜色

		float  m_ftickDelta; //自生成以来的tick
		float  m_fSelfRotAngle; //自身绕Z轴旋转角度 zgz
		MmParticle() : m_ftickDelta(0.0f), m_fSelfRotAngle(0.0f)
		{

		}
	};

}

#endif