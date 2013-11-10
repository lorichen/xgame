uniform sampler2D texture0,texture1,texture2;

varying highp vec2		vTexCoord0;
void main()
{
	highp vec4 c1,a,c2;
	highp vec4 c;
	
	c1 = texture2D(texture0,vTexCoord0);
	a = texture2D(texture1,vTexCoord0);
	c2 = texture2D(texture2,vTexCoord0);
	c = mix(c1,c2,a.r);

	gl_FragColor = c;
}