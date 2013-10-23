
#include "StdAfx.h"
#include "Light.h"


namespace xs 
{
	//-----------------------------------------------------------------------
	Light::Light()
	{
		// Default to point light, white diffuse light, linear attenuation, fair range
		mLightType = LT_POINT;
		mAmbient = ColorValue::Black;
		mDiffuse = ColorValue::White;
		mSpecular = ColorValue::Black;
		mRange = 25;
		mAttenuationConst = 1.0f;
		mAttenuationLinear = 0.0f;
		mAttenuationQuad = 0.0f;

		// Center in world, direction irrelevant but set anyway
		mPosition = Vector3::ZERO;
		mDirection = Vector3::UNIT_Z;

		// Default some spot values
		mSpotInner = Degree(30.0f);
		mSpotOuter = Degree(40.0f);
		mSpotFalloff = 1.0f;

		mEnabled = true;
	}

	//-----------------------------------------------------------------------
	Light::Light(const std::string& name)
	{
		mName = name;

		// Default to point light, white diffuse light, linear attenuation, fair range
		mLightType = LT_POINT;
		mAmbient = ColorValue::Black;
		mDiffuse = ColorValue::White;
		mSpecular = ColorValue::Black;
		mRange = 25;
		mAttenuationConst = 1.0f;
		mAttenuationLinear = 0.0f;
		mAttenuationQuad = 0.0f;

		// Center in world, direction irrelevant but set anyway
		mPosition = Vector3::ZERO;
		mDirection = Vector3::UNIT_Z;

		// Default some spot values
		mSpotInner = Degree(30.0f);
		mSpotOuter = Degree(40.0f);
		mSpotFalloff = 1.0f;

		mEnabled = true;
	}
	//-----------------------------------------------------------------------
	Light::~Light()
	{
	}
	//-----------------------------------------------------------------------
	void  Light::setType(LightType type)
	{
		mLightType = type;
	}
	//-----------------------------------------------------------------------
	LightType Light::getType(void ) const
	{
		return mLightType;
	}
	//-----------------------------------------------------------------------
	void  Light::setPosition(float x, float y, float z)
	{
		mPosition.x = x;
		mPosition.y = y;
		mPosition.z = z;

	}
	//-----------------------------------------------------------------------
	void  Light::setPosition(const Vector3& vec)
	{
		mPosition = vec;
	}
	//-----------------------------------------------------------------------
	const Vector3& Light::getPosition(void ) const
	{
		return mPosition;
	}
	//-----------------------------------------------------------------------
	void  Light::setDirection(float x, float y, float z)
	{
		mDirection.x = x;
		mDirection.y = y;
		mDirection.z = z;
	}
	//-----------------------------------------------------------------------
	void  Light::setDirection(const Vector3& vec)
	{
		mDirection = vec;
	}
	//-----------------------------------------------------------------------
	const Vector3& Light::getDirection(void ) const
	{
		return mDirection;
	}
	//-----------------------------------------------------------------------
	void  Light::setSpotlightRange(const Radian& innerAngle, const Radian& outerAngle, float falloff)
	{

		if (mLightType != LT_SPOTLIGHT)
		{
			Error("setSpotlightRange is only valid for spotlights");
			return;
		}

		mSpotInner =innerAngle;
		mSpotOuter = outerAngle;
		mSpotFalloff = falloff;
	}
	//-----------------------------------------------------------------------
	const Radian& Light::getSpotlightInnerAngle(void ) const
	{
		return mSpotInner;
	}
	//-----------------------------------------------------------------------
	const Radian& Light::getSpotlightOuterAngle(void ) const
	{
		return mSpotOuter;
	}
	//-----------------------------------------------------------------------
	float Light::getSpotlightFalloff(void ) const
	{
		return mSpotFalloff;
	}

	void  Light::setAmbientColor(float red,float green,float blue)
	{
		mAmbient.r = red;
		mAmbient.g = green;
		mAmbient.b = blue;
	}
	void  Light::setAmbientColor(const ColorValue& color)
	{
		mAmbient = color;
	}
	const ColorValue& Light::getAmbientColor()
	{
		return mAmbient;
	}
	//-----------------------------------------------------------------------
	void  Light::setDiffuseColor(float red, float green, float blue)
	{
		mDiffuse.r = red;
		mDiffuse.b = blue;
		mDiffuse.g = green;
	}
	//-----------------------------------------------------------------------
	void  Light::setDiffuseColor(const ColorValue& color)
	{
		mDiffuse = color;
	}
	//-----------------------------------------------------------------------
	const ColorValue& Light::getDiffuseColor(void ) const
	{
		return mDiffuse;
	}
	//-----------------------------------------------------------------------
	void  Light::setSpecularColor(float red, float green, float blue)
	{
		mSpecular.r = red;
		mSpecular.b = blue;
		mSpecular.g = green;
	}
	//-----------------------------------------------------------------------
	void  Light::setSpecularColor(const ColorValue& color)
	{
		mSpecular = color;
	}
	//-----------------------------------------------------------------------
	const ColorValue& Light::getSpecularColor(void ) const
	{
		return mSpecular;
	}
	//-----------------------------------------------------------------------
	void  Light::setAttenuation(float range, float constant,
		float linear, float quadratic)
	{
		mRange = range;
		mAttenuationConst = constant;
		mAttenuationLinear = linear;
		mAttenuationQuad = quadratic;
	}
	//-----------------------------------------------------------------------
	float Light::getAttenuationRange(void ) const
	{
		return mRange;
	}
	//-----------------------------------------------------------------------
	float Light::getAttenuationConstant(void ) const
	{
		return mAttenuationConst;
	}
	//-----------------------------------------------------------------------
	float Light::getAttenuationLinear(void ) const
	{
		return mAttenuationLinear;
	}
	//-----------------------------------------------------------------------
	float Light::getAttenuationQuadric(void ) const
	{
		return mAttenuationQuad;
	}

} // Namespace
