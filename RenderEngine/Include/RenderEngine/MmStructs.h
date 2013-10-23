#ifndef __MmStructs_H__
#define __MmStructs_H__

#pragma pack(push,1)

namespace xs
{
	struct AnimatedVertex
	{
		Vector3 pos;
		uchar weights[4];
		uchar bones[4];
		Vector3 normal;
	};

	struct MmGlobalSequence
	{
		std::vector<int> vGlobalSequences;
	};

	struct MmBoundingVolume
	{
		std::vector<Vector3>	vVertices;
		std::vector<Vector3>	vNormals;
		std::vector<ushort>		vIndices;
	};

	struct MmVertex : public AnimatedVertex
	{
		//Vector3 pos;
		//Uint8 weights[4];
		//Uint8 bones[4];
		//Vector3 normal;
		Vector2 texcoords;
	};

	struct MmFace
	{
		ushort index[3];
	};

	struct MmSub
	{
		ushort vstart;	// first vertex
		ushort vcount;	// num vertices
		ushort istart;	// first index
		ushort icount;	// num indices
		int materialIndex;
		bool animate;
	};

	struct MmTexture
	{
		bool	replacable;
		TextureAddressingMode	swrap,twrap;
		std::string	filename;
	};

	struct MmMaterial
	{
		int	textureIndex;
		bool	twoSide;
		BlendMode	blendMode;
		bool	envmap;
		bool	zwrite;
		bool	unlit;
	};

	enum
	{
	};

	typedef std::pair<uint,uint> AnimRange;
	template<class T>
	struct MmAnimationBlock
	{
		InterpolationType	interpolationType;
		short	globalSequenceID;
		std::vector<AnimRange>	vRanges;
		std::vector<uint> vTimes;
		std::vector<T> vKeys;
	};

	struct MmBone
	{
		short parent;
		bool billboard;
		Vector3 pivot;
		MmAnimationBlock<Vector3> translation;
		MmAnimationBlock<Quaternion> rotation;
		MmAnimationBlock<Vector3> scaling;
	};

	struct MmAnimation
	{
		char name[80];
		uint timeStart;
		uint timeEnd;
		bool loop;
	};

	//ColorAnimation
	struct MmColor
	{
		MmAnimationBlock<Color3> color;
		MmAnimationBlock<short> opacity;
	};

	//TransparencyAnimation
	struct MmTransparency
	{
		MmAnimationBlock<short> transparency;
	};

	//ParticleSystems
	struct SMmParticleSystemParam
	{
		float mid;
		uint colors[3];
		float sizes[3];
		short d[10];
		float unk[3];
		float scales[3];
		float slowdown;
		float rotation;
		float f2[16];
	};

	struct SMmParticleSystem
	{
		int id;
		int flags;
		Vector3 pos;
		short bone;
		std::string texture;
		short blend;
		short type;
		short s1;
		short s2;
		short cols;
		short rows;
		MmAnimationBlock<float> params[10];
		SMmParticleSystemParam p;
	};
}
#pragma pack(pop)


#endif
