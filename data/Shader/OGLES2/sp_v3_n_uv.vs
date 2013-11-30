
attribute highp vec4	position;
attribute highp vec3	normal;
attribute highp vec2	texcoord0;

uniform highp mat4		g_WorldViewProj;
uniform highp vec4		g_Diffuse;

varying highp vec4		vColor;
varying highp vec2		vTex0Coord;

void main(void)
{
	gl_Position = position * g_WorldViewProj;
	
	//gl_Position.x = 0.0;
	//gl_Position.y = 0.0;
	//gl_Position.z = 0.0;
	
	vColor = g_Diffuse;
	
	//vColor.r = 1.0;
	//vColor.g = 0.0;
	//vColor.b = 0.0;
	//vColor.a = 1.0;
	
	vTex0Coord = texcoord0;
}
