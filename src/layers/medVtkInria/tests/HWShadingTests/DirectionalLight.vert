// Shader for directional lights
//
// Example from OpenGL Shading Language book by Randi J. Rost,
// page 218
//

void DirectionalLight(	in int i,
			in vec3 normal,
			inout vec4 ambient,
			inout vec4 diffuse,
			inout vec4 specular)
{
    float nDotVP;	// normal . light direction
    float nDotHV;	// normal . light half vector
    float pf;		// power factor

    nDotVP = max(0.0, dot(normal, vec3 (gl_LightSource[i].position)));
    nDotHV = max(0.0, dot(normal, vec3 (gl_LightSource[i].halfVector)));

    if (nDotVP == 0.0)
	pf = 0.0;
    else
	pf = pow(nDotHV, gl_FrontMaterial.shininess);

    ambient   += gl_LightSource[i].ambient;
    diffuse   += gl_LightSource[i].diffuse * nDotVP;
    specular  += gl_LightSource[i].specular * pf;
}
