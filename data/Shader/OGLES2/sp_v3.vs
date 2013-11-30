
attribute highp vec4	position;

uniform highp mat4		g_WorldViewProj;
uniform highp vec4		g_Diffuse;

varying highp vec4		vColor;

void main(void)
{
	gl_Position = position * g_WorldViewProj ;
	vColor = g_Diffuse;
}
