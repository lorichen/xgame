
attribute highp vec4	position;
attribute highp vec2	texcoord0;

uniform highp mat4		g_WorldViewProj;
uniform highp vec4		g_Diffuse;

varying highp vec2		vTexCoord0;
varying highp vec4		vColor0;

void main(void)
{
	gl_Position = position * g_WorldViewProj ;
	vTexCoord0 = texcoord0;
	vColor0 = g_Diffuse;
}
