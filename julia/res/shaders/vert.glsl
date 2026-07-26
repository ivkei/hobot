#version 330 core
layout (location = 0) in vec2 iPos; //Vertex array layout 0th index

out vec2 vZ;

void main(){
  gl_Position = vec4(iPos, 0, 1.0);
  vZ = iPos;
}
