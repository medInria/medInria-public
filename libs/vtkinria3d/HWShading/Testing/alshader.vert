//varying vec3 lightDir, normal, viewDir, tangent;

void main()
{
  vec3 lightDir, normal, viewDir, tangent;

  vec3 vertexPos = vec3(gl_ModelViewMatrix * gl_Vertex);
  vec3 lightPos = vec3(gl_LightSource[0].position);

  lightDir = normalize(lightPos - vertexPos);		// light direction
  normal = normalize(gl_NormalMatrix * gl_Normal);	// normal
  //reflectDir = normalize(dot(2*normal, lightDir) * normal - lightDir);
							// reflection direction
  viewDir = normalize(vec3(-1.0)* vertexPos); //-viewPos);		// viewing direction
  vec3 tmp = normalize(gl_NormalMatrix*vec3(0, 1, 0)); 	// pointing down.
  tangent = normalize(cross(tmp, normal));

  gl_Position = ftransform();

//  vec3 reflectDir = normalize(dot(2*normal, lightDir) * normal - lightDir); // reflection direction
  vec3 reflectDir = reflect(lightDir, normal);
/*
  float NdotL = max(0.0, dot(normal, lightDir));
  //float NdotH = max(0.0, dot(normal, normalize(vec3(gl_LightSource[0].halfVector))));
  float RdotV = max(0.0, dot(reflectDir, viewDir));
*/

//  vec3 tangent = vec3(0, 1, 0);

  float LdotT = abs(dot(lightDir, tangent));
  float VdotT = abs(dot(viewDir, tangent));
  float NdotL = sqrt(1.0-pow(LdotT, 2.0)); //pow(1-pow(LdotT, 2),0.5);
  float RdotV = max(0.0, NdotL * sqrt(1.0-pow(VdotT,2.0)) - LdotT*VdotT);

  float pf;
  if (NdotL < 0.01)
    pf = 0.0;
  else
    pf = pow(RdotV, gl_FrontMaterial.shininess);
    //pf = pow(NdotH, gl_FrontMaterial.shininess);

  vec4 ambient = gl_LightSource[0].ambient;
  vec4 diffuse = gl_LightSource[0].diffuse * vec4(NdotL);
  vec4 specular = gl_LightSource[0].specular * vec4(pf);
	
  vec4 color = ambient + vec4(0.75)*diffuse + vec4(0.25)*specular;
//  color.b = specular.b;
  //color = specular;
  
//  color.r = NdotL;
//  color.g = 0; color.b = 0;
  color.a = 1.0;
  //color.rgb = reflectDir.xyz;
  //color.rgb = vec3(NdotL);
  //color.r = 2*color.r;

//  color.rgb = vec3(abs(RdotV));
 
  gl_FrontColor = color;

}
