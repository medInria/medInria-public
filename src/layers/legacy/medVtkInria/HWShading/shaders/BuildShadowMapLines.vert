attribute vec2 LineID;
varying vec2 id;

void main()
{
  id = LineID;

//  vec4 pos = gl_ModelViewProjectionMatrix * gl_Vertex;
//  if (LineID.x == 1.0) pos = vec4(100,100,0,1);
//  gl_Position = pos;

  gl_Position = ftransform();
}
