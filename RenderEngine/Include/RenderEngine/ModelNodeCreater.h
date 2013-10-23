#ifndef __ModelNodeCreater_H__
#define __ModelNodeCreater_H__

namespace xs
{
	class ModelNode;

	struct ConfigSubModel
	{
		std::string subModelName;
		std::string subModelTexture;
		BlendMode blendMode;
		bool twoSide;
		ColorValue diffuse;
		bool unShaded;
		bool unFogged;
		bool noDepthSet;
		bool noDepthTest;
		bool sphereEnvironmentMap;
		bool clampS;
		bool clampT;
	};

	struct ConfigAttachment
	{
		std::string boneName;
		std::string modelName;
		Vector3 scale;
		ColorValue diffuse;
		std::string animationName;
		Vector3 rotAxis;
		float	rotAngle;
		Vector3 position;
	};

	struct ConfigModelNode
	{
		std::string name;
		std::string modelName;
		Vector3 scale;
		ColorValue diffuse;
		int stepLength;
		Rect boundingBox;

		std::vector<ConfigSubModel> vSubModels;
		std::vector<ConfigAttachment> vAttachments;
	};

	class _RenderEngineExport ModelNodeCreater
	{
	public:
		static ModelNode*	create(const char* modelName);
		static ModelNode*	create(const ConfigModelNode* modelConfig);
	};
};

#endif