varying vec3 lightDir;
varying vec3 viewDir;

uniform bool RGBColoring;
attribute vec3 Tangent;
varying vec3 ntang; // normalized tangent.

void main()
{
  vec4 realPos = gl_ModelViewMatrix * gl_Vertex; // the vertex position in eye space
  
  lightDir = normalize(gl_LightSource[0].position.xyz - realPos.xyz);
  ntang = normalize(gl_NormalMatrix * Tangent);
  
  // NOTE: is this correct? I think it is at most correct for perspective viewing.
  viewDir = normalize(vec3(-realPos));		// viewing direction

  vec4 color;
  if (RGBColoring) color = vec4(abs(normalize(Tangent)), 1.0);
  else color = gl_Color;

  gl_FrontColor = color;
  gl_Position = ftransform();
}
