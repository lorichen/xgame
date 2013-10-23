#ifndef __CollisionDetection_H__
#define __CollisionDetection_H__

#ifdef SUPPORT_COLLISION_DETECTION
namespace xs
{
	class CollisionDetection
	{
	public:
		Opcode::MeshInterface m_meshInterface;
		Opcode::Model m_model;
	public:
		bool initialize(Vector3 *p,uint ui32Vertices,uint *pIndices,uint ui32Indices);
	};
}
#endif

#endif