uniform sampler2D texture0,texture1,texture2,texture3;

varying highp vec2		vTexCoord0;

void main()
{
	highp vec4 c1,a,c2,c3;
	highp vec4 c;
	
	c1 = texture2D(texture0,vTexCoord0);
	a = texture2D(texture1,vTexCoord0);
	c2 = texture2D(texture2,vTexCoord0);
	c3 = texture2D(texture3,vTexCoord0);
	c = mix(c1,c2,a.b);
	c = mix(c,c3,a.g);

	gl_FragColor = c;
}