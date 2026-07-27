#version 330 core
layout (location = 0) out vec4 oColor;

uniform vec4 uColor;

void main(){
  oColor = uColor;
}
