varying vec2 id;

void main()
{
  vec2 idf = vec2(id[0], id[1]);
  idf = mod(idf, 512.0);
  idf = idf / 512.0;
//  idf = idf / 65536.0;

  vec4 color;
  color.rg = idf;
  color.b = 0.0;
  color.a = 1.0;

  gl_FragColor = color;
}
