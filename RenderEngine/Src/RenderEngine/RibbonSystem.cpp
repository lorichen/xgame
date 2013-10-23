#include "StdAfx.h"
#include "RibbonSystem.h"
#include "Bone.h"
#include "ModelNode.h"
#include "ModelInstance.h"

//飘带系统最多支持的面片数
#define RIBBONSYSTEM_SEGMENT_OVERFLOW 256

namespace xs
{
	RibbonSystem::RibbonSystem()
	{
		m_pData = 0;
		m_pBone = 0;
		m_pTexture = 0;
		m_currentEmission = 0.0f;
		m_pNode = 0;
		m_originTime = 0xFFFFFFFF;
		m_diffuse = Color4::White;
		m_bShowFlag = true;
	}

	RibbonSystem::~RibbonSystem()
	{
		safeRelease(m_pTexture);
	}


	void RibbonSystem::show( bool bShow)
	{
		if( bShow == m_bShowFlag ) return;
		m_bShowFlag = bShow;
		if( !bShow)
		{
			m_vSegments.clear();
		}
	}

	void RibbonSystem::update(const AnimationTime *pTime,float tickDelta,IRenderSystem* pRenderSystem)
	{
		if( !m_pBone || !m_pNode )
			return;

		//如果不显示，且飘带为0，则停止更新
		if( !m_bShowFlag && m_vSegments.size() == 0 )
			return;

		//初始纹理
		if(0 == m_pTexture)
		{
			m_pTexture = pRenderSystem->getTextureManager()->createTextureFromFile(m_pData->textureFilename, FO_LINEAR, FO_LINEAR, FO_NONE, TAM_CLAMP_TO_EDGE, TAM_CLAMP_TO_EDGE);
			if( 0 == m_pTexture )
				return;
		}	

		//判断是否需要消除飘带
		xs::Matrix4 mtxFullTrans = m_pNode->getFullTransform() * m_pBone->getFullTransform();
		xs::Vector3 v3CurPivot = mtxFullTrans * m_pBone->getPivot();
		Vector3 trans, scl;
		Quaternion rot;
		mtxFullTrans.resolve(trans,rot, scl);
		float scale = scl.x;

		//获取飘带的消失速度
		float lifespeed = 1.0f;
		if( m_vSegments.size() > 0 )
		{
			// 计算运动速度
			lifespeed = ( ( (v3CurPivot - m_v3PreviousPivot ).length()/scale) * 1000.0f) / tickDelta;

			if (abs(lifespeed) < 0.05)
			{
				lifespeed = 0.01;
			}
			// 运动越快，消失越慢
			lifespeed = m_pData->fadespeed * 100.0f / lifespeed;
			lifespeed = std::min(1.0f, lifespeed);
		}

		//更新飘带序列
		SegmentList::iterator begin_update = m_vSegments.begin();
		SegmentList::iterator end_update = m_vSegments.end();
		while ( begin_update != end_update )
		{
			(*begin_update).lifeSpan -= lifespeed * tickDelta;
			if( (*begin_update).lifeSpan < 0.0f)
			{
				m_vSegments.pop_front();
				begin_update = m_vSegments.begin();
			}
			else
			{
				float _time = tickDelta /1000.0f;
				(*begin_update).v1 += (*begin_update).v * _time;
				(*begin_update).v2 += (*begin_update).v * _time;
				(*begin_update).v.y += m_pData->gravity * _time;
				++begin_update;				
			}
		}			
		

		//如果显示，才产生新的面片
		if( m_bShowFlag ) 
		{
			//获取飘带的朝向
			Vector3 v3CurUp;
			if( m_pData->enableAssignUpDir )
			{
				//用户指定
				Vector4 v4Up(m_pData->vAssignedUpDir.x, m_pData->vAssignedUpDir.y, m_pData->vAssignedUpDir.z, 1.0f);
				v4Up = mtxFullTrans * v4Up;
				v3CurUp.x = v4Up.x;
				v3CurUp.y = v4Up.y;
				v3CurUp.z = v4Up.z;
			}
			else
			{
				//默认 绑定点到武器中点就是它的方向(和武器平行)
				v3CurUp = v3CurPivot - m_pNode->getFullTransform().getTrans();//r1 * normalizePivot;
			}
			v3CurUp.normalize();

			//生成飘带
			if( m_vSegments.size() == 0 ) //直接插入
			{
				Segment s;
				float heightAbove = scale *m_pData->heightAbove.getFrame(pTime);
				float heightBelow = scale *m_pData->heightBelow.getFrame(pTime);
				s.v1 = v3CurPivot + heightAbove * v3CurUp;
				s.v2 = v3CurPivot + heightAbove * -v3CurUp;
				Color3 c = m_pData->color.getFrame(pTime);
				float alpha = m_pData->alpha.getFrame(pTime);
				s.c.r = c.r;
				s.c.g = c.g;
				s.c.b = c.b;
				s.c.a = alpha;
				s.fullColor = m_diffuse * s.c;
				s.lifeSpan = m_pData->lifeSpan * 1000.0;
				s.v = Vector3(0.0,0.0,0.0);
				m_vSegments.push_back(s);	

				//保存数据
				m_v3PreviousPivot = v3CurPivot;
				m_fPreviousHeightAbove = heightAbove;
				m_fPreviousHeighBelow = heightBelow;
				m_vPreviousUp = v3CurUp;
			}
			else if( m_vSegments.size() == 1 )//线性插值
			{
				Vector3 v3Diff = v3CurPivot - m_v3PreviousPivot;
				float fGenLen =scale * Math::Abs( 1.0/m_pData->emissionRate);//隔多少米产生一个片段
				int iSegNum = xs::Math::Round( v3Diff.length()/ fGenLen );//插入段的数量
				fGenLen = v3Diff.length()/(float)iSegNum;//调整产生距离
				if( iSegNum > 0 )
				{
					//当前的插值参数
					float fCurHeightAbove = scale *m_pData->heightAbove.getFrame(pTime);
					float fCurHeightBelow = scale *m_pData->heightBelow.getFrame(pTime);
					Color3 c3CurColor = m_pData->color.getFrame(pTime);
					float fCurAlpha = m_pData->alpha.getFrame(pTime);

					//计算要插值的参数
					Vector3 vUpRotAxis = m_vPreviousUp.crossProduct( v3CurUp);//旋转轴
					vUpRotAxis.normalize();
					float fCosA = m_vPreviousUp.dotProduct(v3CurUp);//旋转角
					Radian rdAngle = Math::ACos(fCosA);
					Vector3 v3DiffUnit = v3Diff;//单位距离向量
					v3DiffUnit.normalize();

					//初始化插入的segment
					Segment sgInter;	
					float fPreviousLife =  m_vSegments.back().lifeSpan;
					Color4 _preColor = m_vSegments.back().c;
					Color3 c3PreColor(_preColor.r, _preColor.g, _preColor.b);
					float fPreAlpha = m_vSegments.back().c.a;

					//插入segment
					for( int i=0; i <iSegNum; ++i)
					{
						float r = (float)(i+1) / (float)iSegNum;
						//线性插值pivot
						Vector3 v3InterPivot;
						v3InterPivot = m_v3PreviousPivot + v3DiffUnit*( (float)(i+1) * fGenLen );
						//旋转up
						Quaternion qUpRot;
						qUpRot.FromAngleAxis(rdAngle.operator *(r), vUpRotAxis);
						Vector3 v3InterUp = qUpRot * m_vPreviousUp;
						v3InterUp.normalize();
						//插值高和低
						float fInterHeight = m_fPreviousHeightAbove * (1.0 -r) + r* fCurHeightAbove;
						float fInterBelow = m_fPreviousHeighBelow * (1.0-r) + r * fCurHeightBelow;
						//插值颜色和alpha值
						Color3 c3InterColor = c3PreColor * (1.0 -r) + c3CurColor *r;
						float fInterAlpha = fPreAlpha * ( 1.0 -r ) +  fCurAlpha *r;
						//更新segment
						sgInter.v = Vector3(0.0,0.0,0.0);
						sgInter.v1 = v3InterPivot + fInterHeight * v3InterUp;
						sgInter.v2 = v3InterPivot + fInterBelow * v3InterUp.operator -();
						sgInter.c.r = c3InterColor.r;
						sgInter.c.g = c3InterColor.g;
						sgInter.c.b = c3InterColor.b;
						sgInter.c.a = fInterAlpha;
						sgInter.fullColor =m_diffuse *  sgInter.c;
						sgInter.lifeSpan = m_pData->lifeSpan * 1000.0f * r + (1-r)* fPreviousLife;
						m_vSegments.push_back(sgInter);
					}

					//保存数据
					m_v3PrePrePviot = m_v3PreviousPivot + v3DiffUnit*( (float)(iSegNum-1) * fGenLen );
					m_v3PreviousPivot = v3CurPivot;
					m_vPreviousUp = v3CurUp;
					m_fPreviousHeightAbove = fCurHeightAbove;
					m_fPreviousHeighBelow = fCurHeightBelow;			
				}
			}
			else//平滑插值
			{	
				Vector3 v3Diff = v3CurPivot - m_v3PreviousPivot;
				float fGenLen =scale * Math::Abs( 1.0/m_pData->emissionRate);//隔多少米产生一个片段
				int iSegNum = xs::Math::Round( v3Diff.length()/ fGenLen );//插入段的数量
				fGenLen = v3Diff.length()/(float)iSegNum;//调整产生距离
				if(  iSegNum > 0 )
				{
					//当前的插值参数
					float fCurHeightAbove = scale *m_pData->heightAbove.getFrame(pTime);
					float fCurHeightBelow = scale *m_pData->heightBelow.getFrame(pTime);
					Color3 c3CurColor = m_pData->color.getFrame(pTime);
					float fCurAlpha = m_pData->alpha.getFrame(pTime);

					//计算要插值的参数
					Vector3 vUpRotAxis = m_vPreviousUp.crossProduct( v3CurUp);//旋转轴
					vUpRotAxis.normalize();
					float fCosA = m_vPreviousUp.dotProduct(v3CurUp);//旋转角
					Radian rdAngle = Math::ACos(fCosA);
					Vector3 v3DiffUnit = v3Diff;//单位距离向量
					v3DiffUnit.normalize();
					//计算轴点旋转向量
					Vector3 _v3PreDir = m_v3PreviousPivot - m_v3PrePrePviot;
					_v3PreDir.normalize();
					Vector3 _v3CurDir = v3CurPivot - m_v3PreviousPivot;
					_v3CurDir.normalize();
					float _fCosA = _v3PreDir.dotProduct(_v3CurDir);
					Radian rdPivotAngle = Math::ACos(_fCosA);
					//就算轴点旋转轴
					Vector3 v3PivotRot = _v3PreDir.crossProduct(_v3CurDir);
					v3PivotRot.normalize();

					//初始化插入的segment
					Segment sgInter;																												
					float fPreviousLife =  m_vSegments.back().lifeSpan;
					Color4 _preColor = m_vSegments.back().c;
					Color3 c3PreColor(_preColor.r, _preColor.g, _preColor.b);
					float fPreAlpha = m_vSegments.back().c.a;

					//插入segment
					for( int i=0; i <iSegNum; ++i)
					{
						float r;//插值参数
						r = (float)(i+1) / (float)iSegNum;
						//线性插值pivot
						Quaternion qPivotRot;
						qPivotRot.FromAngleAxis( rdPivotAngle * r, v3PivotRot);
						Vector3 v3PivotPosDelta = qPivotRot*_v3PreDir;
						v3PivotPosDelta = v3PivotPosDelta*((float)(i+1) * fGenLen );
						Vector3 v3InterPivot = m_v3PreviousPivot + v3PivotPosDelta;
						//旋转up
						Quaternion qUpRot;
						qUpRot.FromAngleAxis(rdAngle.operator *(r), vUpRotAxis);
						Vector3 v3InterUp = qUpRot * m_vPreviousUp;
						v3InterUp.normalize();
						//插值高和低
						float fInterHeight = m_fPreviousHeightAbove * (1.0 -r) + r* fCurHeightAbove;
						float fInterBelow = m_fPreviousHeighBelow * (1.0-r) + r * fCurHeightBelow;
						//插值颜色和alpha值
						Color3 c3InterColor;
						c3InterColor = c3PreColor * (1.0 -r) + c3CurColor *r;
						float fInterAlpha;
						fInterAlpha = fPreAlpha * ( 1.0 -r ) +  fCurAlpha *r;
						//更新segment
						sgInter.v = Vector3(0.0,0.0,0.0);
						sgInter.v1 = v3InterPivot + fInterHeight * v3InterUp;
						sgInter.v2 = v3InterPivot + fInterBelow * v3InterUp.operator -();
						sgInter.c.r = c3InterColor.r;
						sgInter.c.g = c3InterColor.g;
						sgInter.c.b = c3InterColor.b;
						sgInter.c.a = fInterAlpha;
						sgInter.fullColor =m_diffuse *  sgInter.c;
						sgInter.lifeSpan = m_pData->lifeSpan * 1000.0f * r + (1-r)* fPreviousLife;
						m_vSegments.push_back(sgInter);
					}
					//保存参数
					{
						float r = (float)(iSegNum-1) / (float)iSegNum;
						Quaternion qPivotRot;
						qPivotRot.FromAngleAxis( rdPivotAngle * r, v3PivotRot);
						Vector3 v3PivotPosDelta = qPivotRot*_v3PreDir;
						v3PivotPosDelta = v3PivotPosDelta*((float)(iSegNum-1) * fGenLen );
						m_v3PrePrePviot = m_v3PreviousPivot + v3PivotPosDelta;				
					}
					m_v3PreviousPivot = v3CurPivot;
					m_vPreviousUp = v3CurUp;
					m_fPreviousHeightAbove = fCurHeightAbove;
					m_fPreviousHeighBelow = fCurHeightBelow;			
				}
			}
		}

		//限制片段的个数
		if( m_vSegments.size() > RIBBONSYSTEM_SEGMENT_OVERFLOW)
		{
			SegmentList::iterator _itDel = m_vSegments.begin();
			int _index = 0;
			int _del = m_vSegments.size() - RIBBONSYSTEM_SEGMENT_OVERFLOW;
			for(  ; _index < _del; ++_index)
				++_itDel;
			++_itDel;
			m_vSegments.erase( m_vSegments.begin(), _itDel);			
		}

		//更新片段的纹理坐标
		if(m_vSegments.size() > 1 )
		{
			SegmentList::iterator _itBegin = m_vSegments.begin();
			int _index = 0;
			int _size = m_vSegments.size();
			for( ; _itBegin != m_vSegments.end();++_itBegin)
			{
				(*_itBegin).t1 = Vector2( (float)_index / (float)(_size -1), 0.0);
				(*_itBegin).t2 = Vector2( (float)_index / (float)(_size -1), 1.0);
				++_index;
			}
		}
	}

	void RibbonSystem::saveContext(IRenderSystem *pRenderSystem)
	{
		m_color = pRenderSystem->getColor();
		m_lightingEnabled = pRenderSystem->isLightingEnabled();
		pRenderSystem->getSceneBlending(m_sbfSrc,m_sbfDst);
		m_alphaCheck = pRenderSystem->alphaCheckEnabled();
		pRenderSystem->getAlphaFunction(m_alphaFunc,m_alphaCheckRef);
		m_cullingMode = pRenderSystem->getCullingMode();
		m_bDepthCheck = pRenderSystem->isDepthBufferCheckEnabled();
		m_bDepthWrite = pRenderSystem->isDepthBufferWriteEnabled();
	}

	void RibbonSystem::Clear()
	{
		if(m_pTexture)
		{
			safeRelease(m_pTexture);
			m_pTexture = 0;
		}
		m_vSegments.clear();
		m_currentEmission = 0;
		m_originTime = 0xFFFFFFFF;
		m_diffuse = Color4::White;
	}

	void RibbonSystem::loadContext(IRenderSystem *pRenderSystem)
	{
		pRenderSystem->setColor(m_color);
		pRenderSystem->setLightingEnabled(m_lightingEnabled);
		pRenderSystem->setTexture(0,0);
		pRenderSystem->setSceneBlending(m_sbfSrc,m_sbfDst);
		pRenderSystem->setAlphaCheckEnabled(m_alphaCheck);
		pRenderSystem->setAlphaFunction(m_alphaFunc,m_alphaCheckRef);
		pRenderSystem->setCullingMode(m_cullingMode);
		pRenderSystem->setDepthBufferCheckEnabled(m_bDepthCheck);
		pRenderSystem->setDepthBufferWriteEnabled(m_bDepthWrite);
	}

	void RibbonSystem::render(IRenderSystem *pRenderSystem,bool useMtl)
	{
		PP_BY_NAME("RibbonSystem::render");

		if(!useMtl)
			return;

		if(m_vSegments.size() <= 1)
			return;

		if(!m_pTexture)
			return;

		saveContext(pRenderSystem);

		pRenderSystem->setLightingEnabled(false);
		pRenderSystem->setCullingMode(CULL_NONE);
		pRenderSystem->setTexture(0,m_pTexture);

		switch(m_pData->filterMode)
		{
		case BM_OPAQUE:
			pRenderSystem->setSceneBlending(SBF_ONE,SBF_ZERO);
			pRenderSystem->setAlphaCheckEnabled(false);
			break;
		case BM_TRANSPARENT:
			pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
			pRenderSystem->setAlphaCheckEnabled(true);
			pRenderSystem->setAlphaFunction(CMPF_GREATER,(uchar)0xBE);
			break;
		case BM_ALPHA_BLEND:
			pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
			pRenderSystem->setAlphaCheckEnabled(false);
			break;
		case BM_ADDITIVE:
			pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE);
			pRenderSystem->setAlphaCheckEnabled(false);
			break;
		case BM_ADDITIVE_ALPHA:
			pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE);
			pRenderSystem->setAlphaCheckEnabled(false);
			break;
		case BM_MODULATE:
			pRenderSystem->setSceneBlending(SBF_ZERO,SBF_SOURCE_COLOR);
			pRenderSystem->setAlphaCheckEnabled(false);
			break;
		}

		pRenderSystem->setDepthBufferCheckEnabled(true);
		pRenderSystem->setDepthBufferWriteEnabled(false);

		//Render
		SegmentList::iterator begin = m_vSegments.begin();
		SegmentList::iterator end = m_vSegments.end();

		while( true)
		{
			pRenderSystem->beginPrimitive(PT_TRIANGLE_STRIP);
			while( begin != end)
			{
				Segment &s = (*begin);
				pRenderSystem->setColor(s.c);
				pRenderSystem->setTexcoord(s.t2);
				pRenderSystem->sendVertex(s.v2);
				pRenderSystem->setTexcoord(s.t1);
				pRenderSystem->sendVertex(s.v1);
				++begin;
			}
			pRenderSystem->endPrimitive();
			if( begin == end ) break;
		}

		loadContext(pRenderSystem);
	}

	int RibbonSystem::getRenderOrder()
	{
		switch(m_pData->filterMode)
		{
		case BM_OPAQUE:
			{
				return RENDER_QUEUE_OPAQUE;
			}

		case BM_TRANSPARENT:
			{
				return RENDER_QUEUE_TRANSPARENT;
			}

		case BM_ALPHA_BLEND:
			{
				return RENDER_QUEUE_BLEND;
			}

		case BM_ADDITIVE:
		case BM_ADDITIVE_ALPHA:
		case BM_MODULATE:
			{
				return RENDER_QUEUE_FX;
			}
		}

		return RENDER_QUEUE_OPAQUE;
	}

	void RibbonSystem::reset()
	{
		m_vSegments.clear();
	}

	void RibbonSystem::setColor(const ColorValue& color)
	{
		m_diffuse = color;
	}

	int RibbonSystem::__getRibbonSegNum()
	{
		return m_vSegments.size();
	}
}