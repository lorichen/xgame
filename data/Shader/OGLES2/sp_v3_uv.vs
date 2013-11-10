
attribute highp vec4	position;
attribute highp vec2	texcoord0;

uniform highp mat4		g_WorldViewProj;

varying highp vec2		vTexCoord0;

void main(void)
{
	gl_Position = position * g_WorldViewProj ;
	vTexCoord0 = texcoord0;
}
