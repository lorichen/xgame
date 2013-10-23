#include "StdAfx.h"
#include "CollisionDetection.h"

#ifdef SUPPORT_COLLISION_DETECTION

namespace xs
{
	bool CollisionDetection::initialize(Vector3 *pVertices,uint ui32Vertices,uint *pIndices,uint ui32Indices)
	{
		if(!ui32Indices)return false;

		m_meshInterface.SetNbTriangles(ui32Indices / 3);
		m_meshInterface.SetNbVertices(ui32Vertices);
		m_meshInterface.SetPointers((const IndexedTriangle*)(void*)pIndices,(const IceMaths::Point*) pVertices);

		Opcode::OPCODECREATE OPCC;
		OPCC.mIMesh = &m_meshInterface;
		//OPCC.mSettings
		OPCC.mNoLeaf = false;
		OPCC.mQuantized = false;
		OPCC.mKeepOriginal = false;
		OPCC.mCanRemap = false;
		return m_model.Build(OPCC);
	}

}

#endif