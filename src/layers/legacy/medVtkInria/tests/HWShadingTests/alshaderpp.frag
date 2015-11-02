varying vec3 lightDir, normal, viewDir, tangent;

void main()
{
  float LdotT = dot(lightDir, tangent);
  float VdotT = dot(viewDir, tangent);
  float NdotL = sqrt(1.0-pow(LdotT, 2.0));
  float RdotV = max(0.0, NdotL * sqrt(1.0-pow(VdotT,2.0)) - LdotT*VdotT);
  float pf;

  if (NdotL < 0.01)
    pf = 0.0;
  else
    pf = pow(RdotV, gl_FrontMaterial.shininess);

  vec4 ambient = gl_LightSource[0].ambient;
  vec4 diffuse = gl_LightSource[0].diffuse * NdotL;
  vec4 specular = gl_LightSource[0].specular * pf;

  vec4 color;

  color = ambient + vec4(0.75)*diffuse + vec4(0.25)*specular;

  color.a = 1.0;
  gl_FragColor = color;
}
