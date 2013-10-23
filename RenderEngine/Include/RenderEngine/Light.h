#ifndef __Light_H__
#define __Light_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/


	/** Representation of a dynamic light source in the scene.
	@remarks
	Lights are added to the scene like any other object. They contain various
	parameters like type, position, attenuation (how light intensity fades with
	distance), color etc.
	@par
	The defaults when a light is created is pure white diffues light, with no
	attenuation (does not decrease with distance) and a range of 1000 world units.
	@par
	Lights are created by using the SceneManager::createLight method. They can subsequently be
	added to a SceneNode if required to allow them to move relative to a node in the scene. A light attached
	to a SceneNode is assumed to havea base position of (0,0,0) and a direction of (0,0,1) before modification
	by the SceneNode's own orientation. If not attached to a SceneNode,
	the light's position and direction is as set using setPosition and setDirection.
	@par
	Remember also that dynamic lights rely on modifying the color of vertices based on the position of
	the light compared to an object's vertex normals. Dynamic lighting will only look good if the
	object being lit has a fair level of tesselation and the normals are properly set. This is particularly
	true for the spotlight which will only look right on highly tesselated models. In the future rkt may be
	extended for certain scene types so an alternative to the standard dynamic lighting may be used, such
	as dynamic lightmaps.
	*/
	class _RenderEngineExport Light
	{
	public:
		/// Temp tag used for sorting
		float tempSquareDist;

		/** Default constructor (for Python mainly).
		*/
		Light();

		/** Standard destructor.
		*/
		~Light();

		/** Normal constructor. Should not be called directly, but rather the SceneManager::createLight method should be used.
		*/
		Light(const std::string& name);

		/** Sets the type of light - see LightType for more info.
		*/
		void  setType(LightType type);

		/** Returns the light type.
		*/
		LightType getType(void ) const;

		void  setAmbientColor(float red,float green,float blue);
		void  setAmbientColor(const ColorValue& color);
		const ColorValue& getAmbientColor();
		/** Sets the color of the diffuse light given off by this source.
		@remarks
		Material objects have ambient, diffuse and specular values which indicate how much of each type of
		light an object reflects. This value denotes the amount and color of this type of light the light
		exudes into the scene. The actual appearance of objects is a combination of the two.
		@par
		Diffuse light simulates the typical light emenating from light sources and affects the base color
		of objects together with ambient light.
		*/
		void  setDiffuseColor(float red, float green, float blue);

		/** Sets the color of the diffuse light given off by this source.
		@remarks
		Material objects have ambient, diffuse and specular values which indicate how much of each type of
		light an object reflects. This value denotes the amount and color of this type of light the light
		exudes into the scene. The actual appearance of objects is a combination of the two.
		@par
		Diffuse light simulates the typical light emenating from light sources and affects the base color
		of objects together with ambient light.
		*/
		void  setDiffuseColor(const ColorValue& color);

		/** Returns the color of the diffuse light given off by this light source (see setDiffuseColor for more info).
		*/
		const ColorValue& getDiffuseColor(void ) const;

		/** Sets the color of the specular light given off by this source.
		@remarks
		Material objects have ambient, diffuse and specular values which indicate how much of each type of
		light an object reflects. This value denotes the amount and color of this type of light the light
		exudes into the scene. The actual appearance of objects is a combination of the two.
		@par
		Specular light affects the appearance of shiny highlights on objects, and is also dependent on the
		'shininess' Material value.
		*/
		void  setSpecularColor(float red, float green, float blue);

		/** Sets the color of the specular light given off by this source.
		@remarks
		Material objects have ambient, diffuse and specular values which indicate how much of each type of
		light an object reflects. This value denotes the amount and color of this type of light the light
		exudes into the scene. The actual appearance of objects is a combination of the two.
		@par
		Specular light affects the appearance of shiny highlights on objects, and is also dependent on the
		'shininess' Material value.
		*/
		void  setSpecularColor(const ColorValue& color);

		/** Returns the color of specular light given off by this light source.
		*/
		const ColorValue& getSpecularColor(void ) const;

		/** Sets the attenuation parameters of the light source ie how it diminishes with distance.
		@remarks
		Lights normally get fainter the further they are away. Also, each light is given a maximum range
		beyond which it cannot affect any objects.
		@par
		Light attentuation is not applicable to directional lights since they have an infinite range and
		constant intensity.
		@par
		This follows a standard attenuation approach - see any good 3D text for the details of what they mean
		since i don't have room here!
		@param
		range The absolute upper range of the light in world units
		@param
		constant The constant factor in the attenuation formula: 1.0 means never attenuate, 0.0 is complete attenuation
		@param
		linear The linear factor in the attenuation formula: 1 means attenuate evenly over the distance
		@param
		quadratic The quadratic factor in the attenuation formula: adds a curvature to the attenuation formula.
		*/
		void  setAttenuation(float range, float constant, float linear, float quadratic);

		/** Returns the absolute upper range of the light.
		*/
		float getAttenuationRange(void ) const;

		/** Returns the constant factor in the attenuation formula.
		*/
		float getAttenuationConstant(void ) const;

		/** Returns the linear factor in the attenuation formula.
		*/
		float getAttenuationLinear(void ) const;

		/** Returns the quadric factor in the attenuation formula.
		*/
		float getAttenuationQuadric(void ) const;

		/** Sets the position of the light.
		@remarks
		Applicable to point lights and spotlights only.
		@note
		This will be overridden if the light is attached to a SceneNode.
		*/
		void  setPosition(float x, float y, float z);

		/** Sets the position of the light.
		@remarks
		Applicable to point lights and spotlights only.
		@note
		This will be overridden if the light is attached to a SceneNode.
		*/
		void  setPosition(const Vector3& vec);

		/** Returns the position of the light.
		@note
		Applicable to point lights and spotlights only.
		*/
		const Vector3& getPosition(void ) const;

		/** Sets the direction in which a light points.
		@remarks
		Applicable only to the spotlight and directional light types.
		@note
		This will be overridden if the light is attached to a SceneNode.
		*/
		void  setDirection(float x, float y, float z);

		/** Sets the direction in which a light points.
		@remarks
		Applicable only to the spotlight and directional light types.
		@note
		This will be overridden if the light is attached to a SceneNode.
		*/
		void  setDirection(const Vector3& vec);

		/** Returns the light's direction.
		@remarks
		Applicable only to the spotlight and directional light types.
		*/
		const Vector3& getDirection(void ) const;

		/** Sets the range of a spotlight, i.e. the angle of the inner and outer cones and the rate of falloff between them.
		@param
		innerAngle Angle covered by the bright inner cone
		@param
		outerAngle Angle covered by the outer cone
		@param
		falloff The rate of falloff between the inner and outer cones. 1.0 means a linear falloff, less means slower falloff, higher means faster falloff.
		*/
		void  setSpotlightRange(const Radian& innerAngle, const Radian& outerAngle, float falloff = 1.0);
#ifndef RE_FORCE_ANGLE_TYPES
		inline void  setSpotlightRange(float innerAngle, float outerAngle, float falloff = 1.0) 
		{
			setSpotlightRange ( Angle(innerAngle), Angle(outerAngle), falloff );
		}
#endif

		/** Returns the angle covered by the spotlights inner cone.
		*/
		const Radian& getSpotlightInnerAngle(void ) const;

		/** Returns the angle covered by the spotlights outer cone.
		*/
		const Radian& getSpotlightOuterAngle(void ) const;

		/** Returns the falloff between the inner and outer cones of the spotlight.
		*/
		float getSpotlightFalloff(void ) const;

		const std::string& getName(){return mName;}

		bool isEnabled(){return mEnabled;}
		void  enable(bool bEnable){mEnabled = bEnable;}

	private:

		LightType mLightType;
		Vector3 mPosition;
		ColorValue mAmbient;
		ColorValue mDiffuse;
		ColorValue mSpecular;

		Vector3 mDirection;

		Radian mSpotOuter;
		Radian mSpotInner;
		float mSpotFalloff;
		float mRange;
		float mAttenuationConst;
		float mAttenuationLinear;
		float mAttenuationQuad;

		std::string	mName;
		bool	mEnabled;
	};
	/** @} */
}

#endif
