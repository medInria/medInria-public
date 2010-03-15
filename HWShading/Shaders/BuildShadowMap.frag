void main()
{
  vec4 color;
  //vec4 color = vec4(gl_FragCoord.z, 0.5, (gl_FragCoord.z-gl_DepthRange.near)/gl_DepthRange.diff, 1.0);

//  color.z = gl_FragCoord.z;
//  color.xy = vec2(0, 0);
//  color.a = 1.0;
 // color.rgb = gl_FragCoord.xyz;
//  color.r = 0.0;
//  color.g = 0.0;
//  color.b = gl_FragCoord.z;
  color.rgb = vec3(1, 0, gl_FragCoord.z);
  color.a = 1.0;

  gl_FragColor = color;
}
