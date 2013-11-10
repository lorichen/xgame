
uniform sampler2D 		texture0;


varying highp vec4		vColor;
varying highp vec2		vTex0Coord;

void main (void)
{
	//gl_FragColor = texture2D(texture0,vTex0Coord) * vColor;
	//gl_FragColor = vColor;
	gl_FragColor = texture2D(texture0,vTex0Coord);
}
