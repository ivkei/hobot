#version 330 core
layout (location = 0) out vec4 oColor;

in vec4 vColor;

uniform vec2 uMousePos;
uniform vec2 uWindowDim;

const vec4 LIGHT_COLOR = vec4(1, 1, 1, 1);

void main(){
  vec2 fragPos = gl_FragCoord.xy;
  fragPos.y = -(fragPos.y - uWindowDim.y/2.0) + uWindowDim.y/2.0; //Invert the y

  float d = sqrt((uMousePos.x-fragPos.x)*(uMousePos.x-fragPos.x) + (uMousePos.y-fragPos.y)*(uMousePos.y-fragPos.y));

  float intensity = 0.0;
  if (abs(d) < 1e-5) intensity = 1.0; //Div by 0
  else               intensity = 1.0/d;

  //Smooth out intensity
  intensity *= 50;

  oColor = clamp(vColor*intensity, vec4(0, 0, 0, 0), vec4(1, 1, 1, 1));
}
