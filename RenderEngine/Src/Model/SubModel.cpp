#include "StdAfx.h"
#include "SubModel.h"

namespace xs
{
	const char* SubModel::getName()
	{
		return m_name.c_str();
	}

	void  SubModel::setMaterial(const Material& mtl)
	{
		m_mtl = mtl;
	}

	ushort SubModel::getIndexStart()
	{
		return indexStart;
	}

	ushort SubModel::getIndexCount()
	{
		return indexCount;
	}

	ushort SubModel::getVertexStart()
	{
		return vertexStart;
	}

	ushort SubModel::getVertexEnd()
	{
		return vertexEnd;
	}

	const Material&	SubModel::getMaterial()
	{
		return m_mtl;
	}
	
	GeosetAnimation*	SubModel::getGeosetAnimation()
	{
		return &m_GeosetAnimation;
	}
	
	bool	SubModel::isAnimated()
	{
		return m_bAnimated;
	}

	SubModel::SubModel() : indexCount(0),
		indexStart(0),
		vertexEnd(0),
		vertexStart(0),
		m_bAnimated(false)
	{
		m_ValidIndices = 0;
		for( uint i=0; i <VERTEX_BLEND_MATRICES_NUM; ++i)
			m_BindedBonesIndices[i] = VERTEX_BLEND_MAPPINGS_NUM-1;
		for( uint i=0; i<VERTEX_BLEND_MAPPINGS_NUM; ++i)
			m_BonesMappings[i] = VERTEX_BLEND_MATRICES_NUM - 1;
	}

	uchar * SubModel::getBindedBonesIndices(uint & validIndicesNum)
	{
		validIndicesNum = m_ValidIndices;
		return m_BindedBonesIndices;
	}
}