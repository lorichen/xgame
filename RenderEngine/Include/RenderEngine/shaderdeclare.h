#ifndef __SHADER_DECLARE_H__
#define __SHADER_DECLARE_H__

#include "VertexData.h"

namespace xs
{
	//inner shader vetext enum
	enum ShaderVetextEnum 
	{
		ESV_V3_UV_C = 0,
		ESV_V3_UV,
		ESV_V3,
		ESV_V3_N_UV,

		EV_NUM
	};

	/*
	//ESV_V3_UV_C vertex attr define
	const VertexElementDecs g_v3_uv_c_desc[3] = 
	{
		{VET_FLOAT3,VES_POSITION,0,0},
		{VET_FLOAT2,VES_TEXTURE_COORDINATES,12,0},
		{VET_COLOR,VES_COLOR,20,0},
	};

	//ESV_V3_UV vertex attr define
	const VertexElementDecs g_v3_uv_desc[2] = 
	{
		{VET_FLOAT3,VES_POSITION,0,0},
		{VET_FLOAT2,VES_TEXTURE_COORDINATES,12,0},
	};

	//ESV_V3 vertex attr define
	const VertexElementDecs g_v3_desc[1] = 
	{
		{VET_FLOAT3,VES_POSITION,0,0},
	};
	*/
	
	//------------------------------------------------------------
	
	//统一shader uniform名
	#define G_WORLD_VIEW_PROJ "g_WorldViewProj"
	#define G_WORLD			  "g_World"
	#define G_COLOR			  "g_Color"
	#define G_TEXTURE_0		  "texture0"
	#define G_TEXTURE_1		  "texture1"
	#define G_TEXTURE_2		  "texture2"
	#define G_TEXTURE_3		  "texture3"
	#define G_TEXTURE_4		  "texture4"
	#define G_DIFFUSE		  "g_Diffuse"
	#define G_LIGHT_DIR		  "g_LightDir"

	//---------------------------------------------------------
	
	//常用的shaderprogram名
	#define VS  ".vs"
	#define FS	".fs"

	//inner shader enum
	//有可能是一种ShaderVetextEnum 对应多个shader program enum
	enum ShaderProgramEnum
	{
		ESP_V3_UV_C = 0,
		ESP_V3_UV,
		ESP_V3,
		ESP_V3_N_UV,
		ESP_V3_UV_GC,

		ESP_NUM,
	};

	const char* const g_shader_program_name[] = 
	{
		"sp_v3_uv_c",	
		"sp_v3_uv",		
		"sp_v3",		
		"sp_v3_n_uv",
		"sp_v3_uv_gc",
	};

}

#endif