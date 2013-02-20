//varying vec3 normal; //, lightDir;

void main()
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	int i = 0;
	//for (i=0; i < 300; i++) {
	while (i < 400)
	{
	  color.r = color.r + 0.0025;
	  i++;
	}

	if (i > 250) color.g = 1.0;

	//gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
	gl_FragColor = color;
}
