varying vec3 lightDir, normal, viewDir, tangent;

void main()
{
  //vec3 lightDir, normal, viewDir, tangent;

  vec3 vertexPos = vec3(gl_ModelViewMatrix * gl_Vertex);
  vec3 lightPos = vec3(gl_LightSource[0].position);

  lightDir = normalize(lightPos - vertexPos);		// light direction
  normal = normalize(gl_NormalMatrix * gl_Normal);	// normal
  //reflectDir = normalize(dot(2*normal, lightDir) * normal - lightDir);
							// reflection direction
  viewDir = normalize(-vertexPos); //-viewPos);		// viewing direction
  vec3 tmp = normalize(gl_NormalMatrix*vec3(0, 1, 0)); 	// pointing down.
  tangent = normalize(cross(tmp, normal));

  gl_Position = ftransform();
}
