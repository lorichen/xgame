#include "stdafx.h"
#include "ConfigCreatures.h"
#include "ConfigCreatureRes.h"

ConfigCreatures::~ConfigCreatures()
{
	STLDeleteAssociate(m_creatures);
}

std::string getRes(int id)
{
	ConfigCreatureRes::ResMap::iterator it = ConfigCreatureRes::Instance()->m_resMap.find(id);
	if(it == ConfigCreatureRes::Instance()->m_resMap.end())return "";

	return (*it).second;
}

bool ConfigCreatures::OnSchemeLoad(ICSVReader * pCSVReader,LPCSTR szFileName)
{
	return false;
}

bool ConfigCreatures::OnSchemeLoad(TiXmlDocument * pTiXmlDocument,LPCSTR szFileName)
{
	STLDeleteAssociate(m_creatures);
	TiXmlElement* root = pTiXmlDocument->RootElement();
	if(!root)return false;

	for(TiXmlElement* child = root->FirstChildElement();child;child=child->NextSiblingElement())
	{
		ConfigCreature *cc = new ConfigCreature;

		int cid;
		child->Attribute("ID",&cid);

		cc->name = child->Attribute("Name");
		cc->basicCreatureName = child->Attribute("BasicCreatureName");
		//cc.modelName = child->Attribute("ModelFileName");
		int ResId;
		child->Attribute("ResId",&ResId);
		cc->resId = child->Attribute("ResId");
		cc->modelName = getRes(ResId);

		double temp;
		child->Attribute("ScaleX",&temp);cc->scale.x = temp;
		child->Attribute("ScaleY",&temp);cc->scale.y = temp;
		child->Attribute("ScaleZ",&temp);cc->scale.z = temp;
		child->Attribute("ColorR",&temp);cc->diffuse.r = temp;
		child->Attribute("ColorG",&temp);cc->diffuse.g = temp;
		child->Attribute("ColorB",&temp);cc->diffuse.b = temp;
		child->Attribute("Transparency",&temp);cc->diffuse.a = temp;
		child->Attribute("StepLength",&cc->stepLength);
		int intTemp = 0;
		child->Attribute("ShowRectLeft",&intTemp);
		cc->boundingBox.left = intTemp;
		child->Attribute("ShowRectTop",&intTemp);
		cc->boundingBox.top = intTemp;
		child->Attribute("ShowRectRight",&intTemp);
		cc->boundingBox.right = intTemp;
		child->Attribute("ShowRectBottom",&intTemp);
		cc->boundingBox.bottom = intTemp;

		TiXmlElement *pSubModel = child->FirstChildElement();
		if(pSubModel)
		{
			for(TiXmlElement *child = pSubModel->FirstChildElement();child;child = child->NextSiblingElement())
			{
				ConfigSubModel csm;
				csm.subModelName = child->Attribute("SubModelName");
				//csm.subModelTexture = child->Attribute("SubModelTexture");
				int ResId;
				child->Attribute("ResId",&ResId);
				csm.subModelTexture = getRes(ResId);
				int temp;
				child->Attribute("BlendMode",&temp);
				csm.blendMode = (xs::BlendMode)temp;
				child->Attribute("DoubleSide",&temp);
				csm.twoSide = (temp > 0);

				double tempd;
				child->Attribute("ColorR",&tempd);csm.diffuse.r = tempd;
				child->Attribute("ColorG",&tempd);csm.diffuse.g = tempd;
				child->Attribute("ColorB",&tempd);csm.diffuse.b = tempd;
				child->Attribute("Transparency",&tempd);csm.diffuse.a = tempd;
				child->Attribute("UnShaded",&temp);csm.unShaded = (temp != 0);
				child->Attribute("UnFogged",&temp);csm.unFogged = (temp != 0);
				child->Attribute("NoDepthSet",&temp);csm.noDepthSet = (temp != 0);
				child->Attribute("NoDepthTest",&temp);csm.noDepthTest = (temp != 0);
				child->Attribute("SphereEnvMap",&temp);csm.sphereEnvironmentMap = (temp != 0);
				child->Attribute("ClampS",&temp);csm.clampS = (temp != 0);
				child->Attribute("ClampT",&temp);csm.clampT = (temp != 0);
				cc->vSubModels.push_back(csm);
			}
			TiXmlElement *pAttachment = pSubModel->NextSiblingElement();
			if(pAttachment)
			{
				for(const TiXmlElement *child = pAttachment->FirstChildElement();child;child = child->NextSiblingElement())
				{
					ConfigAttachment ca;
					ca.boneName = child->Attribute("BoneName");
					//ca.modelName = child->Attribute("ModelName");
					int ResId;
					child->Attribute("ResId",&ResId);
					ca.modelName = getRes(ResId);
					double tempd;
					child->Attribute("ScaleX",&tempd);ca.scale.x = tempd;
					child->Attribute("ScaleY",&tempd);ca.scale.y = tempd;
					child->Attribute("ScaleZ",&tempd);ca.scale.z = tempd;
					ca.animationName = child->Attribute("Animation");

					child->Attribute("ColorR",&tempd);ca.diffuse.r = tempd;
					child->Attribute("ColorG",&tempd);ca.diffuse.g = tempd;
					child->Attribute("ColorB",&tempd);ca.diffuse.b = tempd;
					child->Attribute("Transparency",&tempd);ca.diffuse.a = tempd;

					//设置tempd为了与以前的兼容，以前没有这些属性
					int _error;
					_error = child->QueryDoubleAttribute("RotAxisX", &tempd); 
					if(_error ==TIXML_SUCCESS )ca.rotAxis.x = tempd;
					else ca.rotAxis.x = 1;
					_error = child->QueryDoubleAttribute("RotAxisY", &tempd);
					if( _error == TIXML_SUCCESS ) ca.rotAxis.y = tempd;
					else ca.rotAxis.y = 0;
					_error = child->QueryDoubleAttribute("RotAxisZ", &tempd); 
					if( _error == TIXML_SUCCESS) ca.rotAxis.z = tempd;
					else ca.rotAxis.z = 0;
					_error = child->QueryDoubleAttribute("RotAngle", &tempd); 
					if(_error == TIXML_SUCCESS) ca.rotAngle = tempd;
					else ca.rotAngle =0;

					_error = child->QueryDoubleAttribute("PosX", &tempd); 
					if(_error ==TIXML_SUCCESS )ca.position.x = tempd;
					else ca.position.x = 0;
					_error = child->QueryDoubleAttribute("PosY", &tempd);
					if( _error == TIXML_SUCCESS ) ca.position.y = tempd;
					else ca.position.y = 0;
					_error = child->QueryDoubleAttribute("PosZ", &tempd); 
					if( _error == TIXML_SUCCESS) ca.position.z = tempd;
					else ca.position.z = 0;

					cc->vAttachments.push_back(ca);
				}
			}

			TiXmlElement *pAttacks = pAttachment->NextSiblingElement();
			if(pAttacks)
			{
				for(const TiXmlElement *child = pAttacks->FirstChildElement();child;child = child->NextSiblingElement())
				{
					const char *name = child->Attribute("Name");
					cc->aa.attacks.push_back(name);
				}
			}
			TiXmlElement *pSpell = pAttacks->NextSiblingElement();
			cc->aa.spell = pSpell->Attribute("Name");
			TiXmlElement *pPrepare = pSpell->NextSiblingElement();
			cc->aa.prepare = pPrepare->Attribute("Name");
			TiXmlElement *pAttacked = pPrepare->NextSiblingElement();
			cc->aa.attacked = pAttacked->Attribute("Name");
			TiXmlElement *pDizz = pAttacked->NextSiblingElement();
			cc->aa.dizz = pDizz->Attribute("Name");
			TiXmlElement *pLeisure = pDizz->NextSiblingElement();
			cc->aa.leisure = pLeisure->Attribute("Name");
			TiXmlElement *pSoundAttack = pLeisure->NextSiblingElement();
			pSoundAttack->Attribute("Id",&cc->aa.soundAttack);
			TiXmlElement *pSoundDie = pSoundAttack->NextSiblingElement();
			pSoundDie->Attribute("Id",&cc->aa.soundDie);
			TiXmlElement *pSoundWound = pSoundDie->NextSiblingElement();
			pSoundWound->Attribute("Id",&cc->aa.soundWound);
			TiXmlElement *pSoundMove = pSoundWound->NextSiblingElement();
			pSoundMove->Attribute("Id",&cc->aa.soundMove);
			TiXmlElement *pSoundFallow = pSoundMove->NextSiblingElement();
			pSoundFallow->Attribute("Id",&cc->aa.soundFallow);
			TiXmlElement *pSoundOther = pSoundFallow->NextSiblingElement();
			pSoundOther->Attribute("Id",&cc->aa.soundOther);
			TiXmlElement *pRideBone = pSoundOther->NextSiblingElement();
			if(pRideBone)
			{			
					cc->cRide.boneName = pRideBone->Attribute("Name");
					const char *pRideAction = pRideBone->Attribute("RideAction");
					if( 0 != pRideAction )
						cc->cRide.rideAction = pRideBone->Attribute("RideAction");
					else 
						cc->cRide.rideAction.clear();					
			}
			
		}
		m_creatures[cid] = cc;
	}

	const TiXmlElement *pMaxId = root->NextSiblingElement();
	if(pMaxId)
	{
		int maxid;
		pMaxId->Attribute("value",&maxid);
	}

	return true;
}

bool ConfigCreatures::OnSchemeUpdate(ICSVReader * pCSVReader, LPCSTR szFileName)
{
	return false;
}

bool ConfigCreatures::OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, LPCSTR szFileName)
{
	return false;
}

bool ConfigCreatures::load(const std::string& filename)
{
	ISchemeEngine *pSchemeEngine = CreateSchemeEngineProc();
	if(pSchemeEngine)
	{
		return pSchemeEngine->LoadScheme(filename.c_str(),this);
	}

	return false;
}

ConfigCreature* ConfigCreatures::getCreature(int id)
{
	CreatureMap::iterator it = m_creatures.find(id);
    if (it == m_creatures.end())
    {
        return NULL;
    }

	return (*it).second;
}

ConfigCreature* ConfigCreatures::getFirstCreature()
{
	m_it = m_creatures.begin();
	return getNextCreature();
}

ConfigCreature* ConfigCreatures::getNextCreature()
{
	if(m_it == m_creatures.end())return 0;

	return (*m_it++).second;
}