uniform float SpecularPower;
uniform float DiffuseContribution;
uniform float SpecularContribution;
uniform float AmbientContribution;

varying vec3 lightDir, viewDir;
varying vec3 ntang; // normalized tangent

vec4 diffuse(in vec3 l,  in vec3 n);
vec4 specular(in vec3 v, in vec3 l, in vec3 t, in float p);

uniform bool ToneShading;
uniform vec3 CoolColor;
uniform vec3 WarmColor;

void main()
{
  float d = diffuse(lightDir, ntang).r;
  float s = specular(viewDir, lightDir, ntang, SpecularPower).r;

  vec4 color;
//  color = (vec4(AmbientContribution) + vec4(DiffuseContribution)*diffuse(lightDir, ntang))*gl_Color + vec4(SpecularContribution)*specular(viewDir, lightDir, ntang, SpecularPower);

  if (ToneShading)
    {
    float val = AmbientContribution + DiffuseContribution*d + SpecularContribution*s;
    color.rgb = vec3(val)*WarmColor + vec3(1.0-val)*CoolColor;
    }
  else
    { // no tone shading
    color = (vec4(AmbientContribution) + vec4(DiffuseContribution)*d)*gl_Color + vec4(SpecularContribution)*s;
    } // else
  color.a = 1.0;

  gl_FragColor = color;
}
