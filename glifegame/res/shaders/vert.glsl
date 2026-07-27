#version 440 core

layout (location = 0) in vec2 iPos;

out vec2 vTexPos;

void main(){
  gl_Position = vec4(iPos, 0, 1);
  vTexPos = vec2((iPos.x+1.0)/2.0, (iPos.y+1.0)/2.0);
}
