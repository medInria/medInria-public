varying vec3 lightDir, normal, viewDir;

void main()
{
  vec3 nn = normalize(normal);
  vec3 reflectDir = normalize(dot(2*nn, lightDir) * nn - lightDir); // reflection direction

  float NdotL = max(0.0, dot(nn, lightDir));
  //float NdotH = max(0.0, dot(nn, normalize(vec3(gl_LightSource[0].halfVector))));
  float RdotV = max(0.0, dot(reflectDir, viewDir));

  float pf;

  if (NdotL < 0.01)
    pf = 0.0;
  else
    pf = pow(RdotV, gl_FrontMaterial.shininess);
    //pf = pow(NdotH, gl_FrontMaterial.shininess);

  vec4 ambient = gl_LightSource[0].ambient;
  vec4 diffuse = gl_LightSource[0].diffuse * NdotL;
  vec4 specular = gl_LightSource[0].specular * pf;
	
  vec4 color = ambient + diffuse + specular;
  
  //color = specular;
  color.a = 1;
  //color.r = 2*color.r;
 
  gl_FragColor = color;
}
