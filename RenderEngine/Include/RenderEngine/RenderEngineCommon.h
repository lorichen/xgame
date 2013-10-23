#ifndef __RenderEngineCommon_H__
#define __RenderEngineCommon_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	enum RenderMode
	{
		RENDER_MODE_NORMAL,
		RENDER_MODE_SELECTION
	};

	enum
	{
		RENDER_QUEUE_OPAQUE,
		RENDER_QUEUE_TRANSPARENT,
		RENDER_QUEUE_BLEND,
		RENDER_QUEUE_FX,
		RENDER_QUEUE_COUNT
	};

	/** Billboard type. */
	enum BillboardType
	{
		BILLBOARD_NOT_USED,
		BILLBOARD_LOCK_NONE,
		BILLBOARD_LOCK_X,
		BILLBOARD_LOCK_Y,
		BILLBOARD_LOCK_Z
	};

	/** Interpolation type. */
	enum InterpolationType
	{
		INTERPOLATION_NONE,
		INTERPOLATION_LINEAR,
		INTERPOLATION_HERMITE,
		INTERPOLATION_BEZIER
	};

	#define RE_MAX_TEXTURE_COORD_SETS	8
	#define RE_SHADOW_TRANSPARENCY		64
	#define RE_MAX_SIMULTANEOUS_LIGHTS	8

	/** An enumeration describing which material properties should track the vertex colours */
	enum TrackVertexColorEnum
	{
		TVC_NONE        = 0x0,
		TVC_AMBIENT     = 0x1,        
		TVC_DIFFUSE     = 0x2,
		TVC_SPECULAR    = 0x4,
		TVC_EMISSIVE    = 0x8,
		TVC_AMBIENT_AND_DIFFUSE	=	0x3
	};

	/** Light Type. */
	enum LightType
	{
		/// Point light sources give off light equally in all directions, so require only position not direction
		LT_POINT,
		/// Directional lights simulate parallel light beams from a distant source, hence have direction but no position
		LT_DIRECTIONAL,
		/// Spotlights simulate a cone of light from a source so require position and direction, plus extra values for falloff
		LT_SPOTLIGHT
	};

	/** Fog modes. */
	enum FogMode
	{
		/// No fog. Duh.
		FOG_NONE,
		/// Fog density increases  exponentially from the camera (fog = 1/e^(distance * density))
		FOG_EXP,
		/// Fog density increases at the square of FOG_EXP, i.e. even quicker (fog = 1/e^(distance * density)^2)
		FOG_EXP2,
		/// Fog density increases linearly between the start and end distances
		FOG_LINEAR
	};

	/** Primitive Type. */
	enum PrimitiveType
	{
		PT_LINES,
		PT_LINE_STRIP,
		PT_LINE_LOOP,
		PT_TRIANGLES,
		PT_TRIANGLE_STRIP,
		PT_TRIANGLE_FAN,
		PT_QUADS,
		PT_QUAD_STRIP,
		PT_POINTS
	};

	/** The broad type of detail for rendering. */
	enum FillMode
	{
		/// Only points are rendered.
		FM_POINTS = 1,
		/// Wireframe models are rendered.
		FM_WIREFRAME = 2,
		/// Solid polygons are rendered.
		FM_SOLID = 3
	};

	enum CompareFunction
	{
		CMPF_ALWAYS_FAIL,
		CMPF_ALWAYS_PASS,
		CMPF_LESS,
		CMPF_LESS_EQUAL,
		CMPF_EQUAL,
		CMPF_NOT_EQUAL,
		CMPF_GREATER_EQUAL,
		CMPF_GREATER
	};

	/** Hardware culling modes based on vertex winding.
	This setting applies to how the hardware API culls triangles it is sent. */
	enum CullingMode
	{
		/// Hardware never culls triangles and renders everything it receives.
		CULL_NONE = 1,
		/// Hardware culls triangles whose vertices are listed clockwise in the view (default).
		CULL_CLOCKWISE = 2,
		/// Hardware culls triangles whose vertices are listed anticlockwise in the view.
		CULL_COUNTERCLOCKWISE = 3
	};

	/** Blending factors for manually blending objects with the scene. If there isn't a predefined
	SceneBlendType that you like, then you can specify the blending factors directly to affect the
	combination of object and the existing scene. See Material::setSceneBlending for more details.
	*/
	enum SceneBlendFactor
	{
		SBF_ONE,
		SBF_ZERO,
		SBF_DEST_COLOR,
		SBF_SOURCE_COLOR,
		SBF_ONE_MINUS_DEST_COLOR,
		SBF_ONE_MINUS_SOURCE_COLOR,
		SBF_DEST_ALPHA,
		SBF_SOURCE_ALPHA,
		SBF_ONE_MINUS_DEST_ALPHA,
		SBF_ONE_MINUS_SOURCE_ALPHA

	};

	/** Filtering options for textures / mipmaps. */
	enum FilterOptions
	{
		/// No filtering, used for FILT_MIP to turn off mipmapping
		FO_NONE,
		/// Use the closest pixel
		FO_POINT,
		/// Average of a 2x2 pixel area, denotes bilinear for MIN and MAG, trilinear for MIP
		FO_LINEAR,
		/// Similar to FO_LINEAR, but compensates for the angle of the texture plane
		FO_ANISOTROPIC
	};

	/** Blend modes for models / particle systems / ribbon system. */
	enum BlendMode
	{
		BM_OPAQUE,
		BM_TRANSPARENT,
		BM_ALPHA_BLEND,
		BM_ADDITIVE,
		BM_ADDITIVE_ALPHA,
		BM_MODULATE,
		BM_MODULATE2X,
		BM_OPAQUE_NOWRITEDEPTH//不写深度缓冲
	};

	/** Texture addressing mode. */
	enum TextureAddressingMode
	{
		/// Texture wraps at values over 1.0
		TAM_WRAP,
		/// Texture mirrors (flips) at joins over 1.0
		TAM_MIRROR,
		/// Texture clamps at 1.0
		TAM_CLAMP_TO_EDGE,
		//
		TAM_CLAMP
	};

	/// Enum describing the ways to generate texture coordinates
	enum TexCoordCalcMethod
	{
		/// No calculated texture coordinates
		TEXCALC_NONE,
		/// Environment map based on vertex normals
		TEXCALC_ENVIRONMENT_MAP,
		/// Environment map based on vertex positions
		TEXCALC_ENVIRONMENT_MAP_PLANAR,
		TEXCALC_ENVIRONMENT_MAP_REFLECTION,
		TEXCALC_ENVIRONMENT_MAP_NORMAL,
		/// Projective texture
		TEXCALC_PROJECTIVE_TEXTURE
	};

	/// Enum describing the various actions which can be taken onthe stencil buffer
	enum StencilOperation
	{
		/// Leave the stencil buffer unchanged
		SOP_KEEP,
		/// Set the stencil value to zero
		SOP_ZERO,
		/// Set the stencil value to the reference value
		SOP_REPLACE,
		/// Increase the stencil value by 1, clamping at the maximum value
		SOP_INCREMENT,
		/// Decrease the stencil value by 1, clamping at 0
		SOP_DECREMENT,
		/// Increase the stencil value by 1, wrapping back to 0 when incrementing the maximum value
		SOP_INCREMENT_WRAP,
		/// Decrease the stencil value by 1, wrapping when decrementing 0
		SOP_DECREMENT_WRAP,
		/// Invert the bits of the stencil buffer
		SOP_INVERT
	};

	/// Texture Stage Operator
	enum TextureStageOperator
	{
		/// Select Arg0
		TSO_REPLACE,
		/// Arg0 * Arg1
		TSO_MODULATE,
		/// (Arg0 * Arg1) << 1
		TSO_MODELATE2X,
		/// Arg0 + Arg1
		TSO_ADD,
		/// Arg0 + Arg1 - 0.5
		TSO_ADD_SIGNED,
		/// Arg0 - Arg1
		TSO_SUBTRACT,
	};

	/// Texture Stage Argument
	enum TextureStageArgument
	{
		/// From Texture
		TSA_TEXTURE,
		/// From Previous
		TSA_PREVIOUS,
	};

	/** @} */
}

#endif