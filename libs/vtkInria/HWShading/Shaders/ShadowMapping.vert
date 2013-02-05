varying vec4 projCoord;
varying vec3 normal;
varying vec3 lightDir;
varying vec3 viewDir;
varying vec4 realPos;

void main()
{
  //vec4 realPos = gl_ModelViewMatrix * gl_Vertex; // the vertex position in eye space
  realPos = gl_ModelViewMatrix * gl_Vertex; // the vertex position in eye space
  
  lightDir = normalize(gl_LightSource[0].position.xyz - realPos.xyz);
  normal = normalize(gl_NormalMatrix * gl_Normal);
  
  projCoord = gl_TextureMatrix[0] * realPos;

  // NOTE: is this correct? I think it is at most correct for perspective viewing.
  viewDir = normalize(vec3(-realPos));		// viewing direction
  
  gl_Position = ftransform();
}
