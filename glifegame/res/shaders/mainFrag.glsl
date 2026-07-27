#version 440 core

layout (location = 0) out vec4 oColor;

layout (rgba32f, location = 0) uniform image2D uTextureBack;
layout (rgba32f, location = 1) uniform image2D uTextureFront; //The texture slots (image2D type for convenience (we are sampling from it))

in vec2 vTexPos;

uniform int uWidth;
uniform int uHeight;
uniform vec4 uColorAlive;
uniform vec4 uColorDead;

bool GetAt(ivec2 pos){
  return imageLoad(uTextureFront, pos) != uColorDead;
}

int NeighborCount(ivec2 pos){
  int count = 0;
  for (int i = -1; i <= 1; i++){
    for (int j = -1; j <= 1; j++){
      int xPrime = (pos.x+i+uWidth)%uWidth;
      int yPrime = (pos.y+j+uHeight)%uHeight;
      if (xPrime == pos.x && yPrime == pos.y) continue;

      if (GetAt(ivec2(xPrime, yPrime))) count++;
    }
  }
  return count;
}

//Assumes the program resets the color of the texture with anything but color alive and dead
const float EPSILON = 0.0001f;
bool EpEq(vec4 v1, vec4 v2){
  return length(v1-v2) < EPSILON;
}

bool ToCompute(ivec2 pos){
  vec4 color = imageLoad(uTextureBack, pos);
  return !EpEq(color, uColorAlive) && !EpEq(color, uColorDead);
}

void Alive(ivec2 pos){
  imageStore(uTextureBack, pos, uColorAlive);
}
void Dead(ivec2 pos){
  imageStore(uTextureBack, pos, uColorDead);
}

void main(){
  ivec2 texPos = ivec2(vTexPos.x*uWidth, vTexPos.y*uHeight);

  //Just render front
  oColor = imageLoad(uTextureFront, texPos);

  //Compute back based on front (only once)
  if (ToCompute(texPos)){
    bool alive = GetAt(texPos);
    int neighborCount = NeighborCount(texPos);

    if (alive && (neighborCount < 2 || neighborCount > 3)) alive = false;
    else if (!alive && neighborCount == 3) alive = true;

    if (alive) Alive(texPos);
    else       Dead(texPos);
  }
}
