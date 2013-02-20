void main()
{
  vec3 vertexPos = vec3(gl_ModelViewMatrix * gl_Vertex);
  vec3 lightPos = vec3(gl_LightSource[0].position);


  vec3 lightDir = normalize(lightPos - vertexPos);		// light direction
  vec3 normal = normalize(gl_NormalMatrix * gl_Normal);		// normal
  vec3 reflectDir = normalize(vec3(dot(vec3(2)*normal, lightDir)) * normal - lightDir);
//  vec3 reflectDir = normal*vec3(2);
								// reflection direction
  vec3 viewDir = normalize(-vertexPos); //-viewPos);		// viewing direction

  vec4 color = vec4(0, 0, 0, 1);

  float pf;           // power factor for specular lighting

  float NdotL = max(0.0, dot(normal, lightDir));
  float NdotH = max(0.0, dot(normal, normalize(vec3(gl_LightSource[0].halfVector))));
  float RdotV = max(0.0, dot(reflectDir, viewDir));
  if (NdotL < 0.01)
    pf = 0.0;
  else
    pf = pow(RdotV, gl_FrontMaterial.shininess);

  vec4 ambient = gl_LightSource[0].ambient;
  vec4 diffuse = gl_LightSource[0].diffuse * vec4(NdotL);
  vec4 specular = gl_LightSource[0].specular * vec4(pf);
	
  color = ambient + vec4(0.75)*diffuse + vec4(0.25)*specular;
  color.a = 1.0;
 
  gl_FrontColor = color;// + vec4(0,0,0,1);
  gl_Position = ftransform();
}
