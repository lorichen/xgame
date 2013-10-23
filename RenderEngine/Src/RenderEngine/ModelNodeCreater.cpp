#include "StdAfx.h"
#include "ModelNodeCreater.h"
#include "ModelNode.h"
#include "ModelInstance.h"
#include "SubModelInstance.h"

namespace xs
{
	ModelNode*	ModelNodeCreater::create(const char *modelName)
	{
		ModelNode *pModelNode = new ModelNode;
		if(pModelNode)
		{
			if(!pModelNode->setModel(modelName))
			{
				delete pModelNode;
				pModelNode = 0;
			}
		}

		return pModelNode;
	}

	ModelNode*	ModelNodeCreater::create(const ConfigModelNode* modelConfig)
	{
		if(!modelConfig)return 0;

		ModelNode *pModelNode = new ModelNode;
		if(!pModelNode)return 0;
		if(!pModelNode->setModel(modelConfig->modelName.c_str()))
		{
			delete pModelNode;
			return 0;
		}
		pModelNode->setBoundingBox(modelConfig->boundingBox);
		pModelNode->setStepLength(modelConfig->stepLength);
		pModelNode->setName(modelConfig->name);
		pModelNode->setScale(modelConfig->scale);
		pModelNode->setGlobalDiffuse(modelConfig->diffuse);
		ModelInstance *pModelInstance = pModelNode->getModelInstance();
		if(1)
		{
			std::vector<ConfigSubModel>::const_iterator begin = modelConfig->vSubModels.begin();
			std::vector<ConfigSubModel>::const_iterator end = modelConfig->vSubModels.end();
			while(begin != end)
			{
				const ConfigSubModel& subModel = (*begin);
				SubModelInstance *pSubModelInstance = pModelInstance->getSubModel(subModel.subModelName.c_str());
				if(pSubModelInstance)
				{
					if(subModel.subModelTexture != "")pSubModelInstance->setTexture(0,subModel.subModelTexture.c_str());
					Material *pMtl = pSubModelInstance->getMaterial();
					MaterialLayer *pLayer = pMtl->getLayer(0);
					pLayer->m_bTwoSide = subModel.twoSide;
					pLayer->m_diffuse = subModel.diffuse;
					pLayer->m_blendMode = subModel.blendMode;
					pLayer->m_bUnshaded = subModel.unShaded;
					pLayer->m_bUnfogged = subModel.unFogged;
					pLayer->m_bNoDepthSet = subModel.noDepthSet;
					pLayer->m_bNoDepthTest = subModel.noDepthTest;
					pLayer->m_SphereEnvironmentMap = subModel.sphereEnvironmentMap;
					pLayer->m_bClampS = subModel.clampS;
					pLayer->m_bClampT = subModel.clampT;
				}
				++begin;
			}
		}
		if(1)
		{
			std::vector<ConfigAttachment>::const_iterator begin = modelConfig->vAttachments.begin();
			std::vector<ConfigAttachment>::const_iterator end = modelConfig->vAttachments.end();
			while(begin != end)
			{
				const ConfigAttachment& attachment = (*begin);
				Bone *pBone = pModelInstance->getBone(attachment.boneName.c_str());
				if(!pBone){++begin;continue;}
				ModelNode *pNode = create(attachment.modelName.c_str());
				if(!pNode){++begin;continue;}
				pModelNode->addChild(pNode,attachment.boneName.c_str());
				ModelInstance *pInstance = pNode->getModelInstance();
				pInstance->setCurrentAnimation(attachment.animationName.c_str());
				pNode->setScale(attachment.scale);
				pNode->setGlobalDiffuse( attachment.diffuse);
				xs::Quaternion rot;
				rot.FromAngleAxis( attachment.rotAngle, attachment.rotAxis);
				pNode->setOrientation(rot);
				pNode->setPosition(attachment.position);
				++begin;
			}
		}

		return pModelNode;
	}
};