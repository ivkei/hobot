#version 330 core
layout (location = 0) out vec4 oColor;

in vec2 vZ;

uniform float uCReal;
uniform float uCImag;
uniform int uIterations;
uniform float uR;

vec4 hsbToRgba(vec3 args){
  vec3 rgb = clamp(abs(mod(args.x*6.0+vec3(0.0, 4.0, 2.0), 6.0)-3.0)-1.0, 0.0, 1.0);
  rgb = rgb*rgb*(3.0-2.0*rgb);
  return vec4(args.z*mix(vec3(1.0), rgb, args.y), 1.0);
}

float pi = 3.141592;

vec4 Color(vec2 z, int i){
  //Approach 1 (Richer color for transitions)
  float smoothI = float(i) + 1.0 - log(log(sqrt(z.x*z.x+z.y*z.y)))/log(uR*uR);
  float a = smoothI/uIterations;
  float b = sin(pi*sin(pi*sin(pi*sin(a*pi))));
  vec4 color = hsbToRgba(vec3(1.0-a, 1.0, b));
  color = vec4(1.0-color.x, 1.0-color.y, 1.0-color.z, 1.0);
  return color;

  //Approach 2 (Smoother with more iterations)
  //float smoothI = float(i) + 1.0 - log(log(sqrt(z.x*z.x+z.y*z.y)))/log(uR*uR);
  //float t = smoothI / 256.0;
  //vec3 col = vec3(
  //  0.5 + 0.5*cos(2*pi*(t+0.33)),
  //  0.5 + 0.5*cos(2*pi*(t+0.67)),
  //  0.5 + 0.5*cos(2*pi*(t+0.0))
  //);
  //return vec4(col, 1.0);
}

void main(){
  //Scale z
  vec2 scaledZ = vec2(vZ.x*uR, vZ.y*uR);

  int i = 0;

  while (scaledZ.x*scaledZ.x+scaledZ.y*scaledZ.y < uR*uR && i < uIterations){
    scaledZ = vec2(scaledZ.x*scaledZ.x - scaledZ.y*scaledZ.y + uCReal, 2*scaledZ.x*scaledZ.y + uCImag);

    i++;
  }

  oColor = Color(scaledZ, i);
}
