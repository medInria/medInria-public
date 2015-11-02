void main()
{
  //gl_FrontColor = vec4(1, 0, 0, 1);

  gl_Position = ftransform();
  //gl_Position = gl_ModelViewProjectionMatrix * a;
  gl_TexCoord[0] = gl_MultiTexCoord0;
}
