
attribute highp vec4	position;
attribute highp vec2	texcoord0;
attribute highp vec4	color0;

uniform highp mat4		g_WorldViewProj;

varying highp vec2		vTexCoord0;
varying highp vec4		vColor0;

void main(void)
{
	gl_Position = position * g_WorldViewProj ;
	//gl_Position.x = 0.0;
	//gl_Position.y = 0.0;
	//gl_Position.z = 0.0;

	vTexCoord0 = texcoord0;
	
    vColor0 = color0;
	//vColor0.r = 1.0;
	//vColor0.g = 0.0;
	//vColor0.b = 0.0;
	//vColor0.a = 1.0;
}
