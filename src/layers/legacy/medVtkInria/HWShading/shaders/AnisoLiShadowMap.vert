varying vec4 projCoord;
varying vec3 lightDir;
varying vec3 viewDir;
//varying vec4 realPos;

uniform bool RGBColoring;
attribute vec3 Tangent;
varying vec3 ntang; // normalized tangent.

attribute vec2 LineID;
varying vec2 id;

void main()
{
  id = LineID;

  vec4 realPos = gl_ModelViewMatrix * gl_Vertex; // the vertex position in eye space
//  realPos = gl_ModelViewMatrix * gl_Vertex; // the vertex position in eye space
  
  lightDir = normalize(gl_LightSource[0].position.xyz - realPos.xyz);
  //normal = normalize(gl_NormalMatrix * gl_Normal);
  ntang = normalize(gl_NormalMatrix * Tangent);
  
  projCoord = gl_TextureMatrix[0] * realPos;

  // NOTE: is this correct? I think it is at most correct for perspective viewing.
  viewDir = normalize(vec3(-realPos));		// viewing direction

  vec4 color;
  if (RGBColoring) color.rgb = abs(normalize(Tangent));
  else color = gl_Color;

  color.a = 1.0;
  //gl_FrontColor = gl_Color;  
  gl_FrontColor = color;

  gl_Position = ftransform();
}
