
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/
#ifndef __MzHeader_H__
#define __MzHeader_H__

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;

typedef struct tranTag
{
	float m_mat[4][3];
} tranMatrix;

typedef struct _modelversion 
{
	uint32 nVersion;
} MODELVERSION;

typedef struct _submesh_v1_3
{
	uint16 matId;			//材质id
	uint16 vstart;			// first vertex
	uint16 vcount;			// num vertices
	uint16 istart;			// first index
	uint16 icount;			// num indices
	bool		animated;
	char name[256];			// 子模型名称
} SUBMESH_V1_3;

/// Vertex
struct _VertexData
{
	float pos[3];
	float normal[3];
	float color[4];
	float weights[4];
	uint8 bones[4];
	float texcoords[2];
};

typedef struct _modelface
{
	int index[3];
} MODELFACE;

typedef struct _modelanimation
{
	std::string name;	// 动画名称
	uint32 startTime;
	uint32 endTime;
	uint32 loopType;
} MODELANIMATION;

struct ModelKeyframeTranslation
{
	int time;
	float v[3];
};

struct ModelKeyframeRotation
{
	int time;
	float q[4];
};

struct ModelKeyframeScale
{
	int time;
	float v[3];
};

typedef struct _modelbone
{
	int id;	
	int parent;		// 父节点的id
	std::string name;
	tranMatrix tran;

	std::vector<ModelKeyframeTranslation> keyframesTranslation;
	std::vector<ModelKeyframeRotation> keyframesRotation;
	std::vector<ModelKeyframeScale> keyframesScale;
} MODELBONE;


struct color
{	
	color(){}
	color(float ca,float cr,float cg,float cb){ a = ca; r = cr; g = cg; b = cb;}
	color &operator =(const color &c) {  r=c.r; g=c.g; b=c.b; a=c.a; return *this; 	}
	float a,r,g,b;
};

typedef struct _material
{
	std::string name;
	bool  lighting;
	color ambient;
	color diffuse;
	color specular;
	color emissive;
	bool  isTransparent;
	bool  twoSide;
}MATERIAL;

#endif
	/** @} */