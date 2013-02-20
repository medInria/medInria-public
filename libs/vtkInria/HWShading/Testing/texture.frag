//varying vec3 normal; //, lightDir;
uniform sampler2D incredibles;

void main()
{
	//vec3 n;
	//vec4 color;
	//n = normalize(normal);

	//color = vec4(abs(n[0]), abs(n[1]), abs(n[2]), 1);
		
	//gl_FragColor = color;
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	vec3 color = vec3 (texture2D(incredibles, gl_TexCoord[0].st));
	//color = color * 20;
/*
	if (color.r > 0.5) if (color.g < 0.5) if (color.b < 0.5)
	  {
	  float b = color.b;
	  float r = color.r;
	  color.r = b;
	  color.b = r;
	  }
	  
	if (color.r > 0.9) if (color.g > 0.5)
	  {
	  float l = (color.r + color.g + color.b)/3.0;
	  color.r = l;
	  color.b = l;
	  color.g = l;
	  }
*/
	//if (color.r/s < 1.0)
	if ((color.r < color.g) || (color.r < color.b) || (color.g > 0.4))
	  {
	  float s = (color.r + color.g + color.b)/3.0;	  
	  color.r = s;
	  color.g = s;
	  color.b = s;
	  }
	else
	  {
	  color.b = color.r; color.g = color.r; color.r = 0;
	  }
	gl_FragColor = vec4(color, 1.0);
}
