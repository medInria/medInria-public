varying vec3 lightDir, normal, viewDir;

void main()
{
  vec3 vertexPos = gl_ModelViewMatrix * gl_Vertex;
  vec3 lightPos = gl_LightSource[0].position;

  lightDir = normalize(lightPos - vertexPos);		// light direction
  normal = normalize(gl_NormalMatrix * gl_Normal);	// normal
  //reflectDir = normalize(dot(2*normal, lightDir) * normal - lightDir);
							// reflection direction
  viewDir = normalize(-vertexPos); //-viewPos);		// viewing direction

  gl_Position = ftransform();
}
