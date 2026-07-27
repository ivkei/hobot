#version 440 core

layout (location = 0) out vec4 oColor;

layout (rgba32f, location = 0) uniform image2D uTexture;

in vec2 vTexPos;

uniform int uWidth;
uniform int uHeight;
uniform vec4 uColorAlive;
uniform vec4 uColorDead;
uniform int uPopulation;
uniform int uSeed;

const float EPSILON = 0.0001f;
bool EpEq(vec4 v1, vec4 v2){
  return length(v1-v2) < EPSILON;
}

bool ToCompute(ivec2 pos){
  vec4 color = imageLoad(uTexture, pos);
  return !EpEq(color, uColorAlive) && !EpEq(color, uColorDead);
}

void Alive(ivec2 pos){
  imageStore(uTexture, pos, uColorAlive);
}
void Dead(ivec2 pos){
  imageStore(uTexture, pos, uColorDead);
}

bool Rand(ivec2 pos){
  return int(floor(2234982.0*(sin(float(uSeed-pos.x*pos.y-pos.x*25+uSeed*pos.x)/237.0)+1.0)))%100 < uPopulation;
}

void main(){
  ivec2 texPos = ivec2(vTexPos.x*uWidth, vTexPos.y*uHeight);

  //Compute (only once)
  if (ToCompute(texPos)){
    if (Rand(texPos)) Alive(texPos);
    else Dead(texPos);
  }

  oColor = imageLoad(uTexture, texPos);
}
