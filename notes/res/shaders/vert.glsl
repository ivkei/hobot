#version 330 core
layout (location = 0) in vec2 iPos; //Vertex array layout 0th index
layout (location = 1) in vec4 iColor;

out vec4 vColor;

void main(){
  gl_Position = vec4(iPos, 0, 1.0);
  vColor = iColor;
}
