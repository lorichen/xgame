uniform sampler2D texture0,texture1,texture2,texture3,texture4;

varying highp vec2		vTexCoord0;

void main()
{
	highp vec4 c1,a,c2,c3,c4;
	highp vec4 c;
	c1 = texture2D(texture0,vTexCoord0);
	a = texture2D(texture1,vTexCoord0);
	c2 = texture2D(texture2,vTexCoord0);
	c3 = texture2D(texture3,vTexCoord0);
	c4 = texture2D(texture4,vTexCoord0);
	c = mix(c1,c2,a.b);
	c = mix(c,c3,a.g);
	c = mix(c,c4,a.r);

	gl_FragColor = c;
}