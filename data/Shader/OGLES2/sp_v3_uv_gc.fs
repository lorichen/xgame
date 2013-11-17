uniform sampler2D 		texture0;

varying highp vec2		vTexCoord0;
varying highp vec4		vColor0;
void main (void)
{
	gl_FragColor = texture2D(texture0,vTexCoord0) * vColor0;;
}
