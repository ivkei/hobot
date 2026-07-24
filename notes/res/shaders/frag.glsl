#version 330 core
layout (location = 0) out vec4 oColor;

in vec4 vColor;

void main(){
  oColor = vColor;
}
