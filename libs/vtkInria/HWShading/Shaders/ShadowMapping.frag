uniform sampler2D ShadowMap;

varying vec4 projCoord;
varying vec3 normal, lightDir, viewDir;

vec4 diffuse(in vec3 l,  in vec3 n);
vec4 specular(in vec3 v, in vec3 l, in vec3 n, float p);

varying vec4 realPos;

void main()
{
  vec4 color;

//  vec4 projCoord2 = gl_TextureMatrix[0] * realPos;

  vec3 projectiveBiased = (projCoord.xyz / projCoord.q);

  // convert from [-1.0, 1.0] to [0.0, 1.0]
  projectiveBiased = (projectiveBiased + 1.0) * 0.5;
  
  vec4 shadowValue = texture2D(ShadowMap, projectiveBiased.xy);

  if (projectiveBiased.z - shadowValue.z < 0.01)
  //if (projectiveBiased.z - shadowValue.z < 0.004)
    { // in light
    //float col = 0.5*diffuse(lightDir, normal) + 0.5*specular(viewDir, lightDir, normal, 4.0);
    //color = vec4(col, col, col, 1.0);
//    color = 0.5*diffuse(lightDir, normal)*gl_Color + 0.5*specular(viewDir, lightDir, normal, 4);
    color = vec4(1, 1, 0, 1);
    }
  else
    { // in shadow
//    color = vec4(0.4, 0.4, 0.4, 1.0) * diffuse(lightDir, normal);
    color = vec4(0.5, 0.5, 0, 1);
    }

//  color[0] = 50*(projectiveBiased.z - shadowValue.z);
//  color[1] = color[0];
//  color[2] = 0;
//  color.rgb = shadowValue.xyz;
  color[3] = 1.0;

  gl_FragColor = color;
}
