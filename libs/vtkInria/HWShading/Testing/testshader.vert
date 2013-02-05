uniform vec3 Color;

void main()
{
  //gl_FrontColor = vec4(Color.r, Color.g, Color.b, 1);
  float v = average(1.0, 0);
  gl_FrontColor = vec4(v, v, v, 1);

  gl_Position = ftransform();
  //gl_Position = gl_ModelViewProjectionMatrix * a;
}
