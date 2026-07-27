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

//Default
//const bool BSNOTATION = true;
//const int NEIGHBOR = 3;
//const int SURVIVALCOUNT = 2;
//const int SURVIVAL[2] = int[](2, 3);
//const int BIRTHCOUNT = 1;
//const int BIRTH[] = int[](3);

//Cubinder
//const bool BSNOTATION = true;
//const int NEIGHBOR = 3;
//const int SURVIVALCOUNT = 4;
//const int SURVIVAL[] = int[](5, 6, 7, 8);
//const int BIRTHCOUNT = 5;
//const int BIRTH[] = int[](3, 5, 6, 7, 8);

//River flow
//const bool BSNOTATION = true;
//const int NEIGHBOR = 3;
//const int SURVIVALCOUNT = 5;
//const int SURVIVAL[] = int[](3, 4, 6, 7, 8);
//const int BIRTHCOUNT = 4;
//const int BIRTH[] = int[](3, 6, 7, 8);

//Evaporation
//const bool BSNOTATION = true;
//const int NEIGHBOR = 3;
//const int SURVIVALCOUNT = 5;
//const int SURVIVAL[] = int[](3, 5, 6, 7, 8);
//const int BIRTHCOUNT = 4;
//const int BIRTH[] = int[](4, 6, 7, 8);

//Window ice
//const bool BSNOTATION = true;
//const int NEIGHBOR = 3;
//const int SURVIVALCOUNT = 5;
//const int SURVIVAL[] = int[](4, 5, 6, 7, 8);
//const int BIRTHCOUNT = 1;
//const int BIRTH[] = int[](3);

//Blobs
//const bool BSNOTATION = false;
//const int NEIGHBOR = 11;
//const int SURVIVALCOUNT = 2;
//const int SURVIVAL[] = int[](33, 57);
//const int BIRTHCOUNT = 2;
//const int BIRTH[] = int[](34, 45);

//Rockets
const bool BSNOTATION = false;
const int NEIGHBOR = 5;
const int SURVIVALCOUNT = 2;
const int SURVIVAL[] = int[](4, 4);
const int BIRTHCOUNT = 2;
const int BIRTH[] = int[](4, 4);

//Islands
//const bool BSNOTATION = false;
//const int NEIGHBOR = 11;
//const int SURVIVALCOUNT = 2;
//const int SURVIVAL[] = int[](41, 119);
//const int BIRTHCOUNT = 2;
//const int BIRTH[] = int[](4, 5);

int NeighborCount(ivec2 pos){
  int count = 0;
  int t = NEIGHBOR/2;
  for (int i = -t; i <= t; i++){
    for (int j = -t; j <= t; j++){
      int xPrime = (pos.x+i+uWidth)%uWidth;
      int yPrime = (pos.y+j+uHeight)%uHeight;
      if (xPrime == pos.x && yPrime == pos.y) continue;

      if (GetAt(ivec2(xPrime, yPrime))) count++;
    }
  }
  return count;
}

void main(){
  ivec2 texPos = ivec2(vTexPos.x*uWidth, vTexPos.y*uHeight);

  //Just render front
  oColor = imageLoad(uTextureFront, texPos);

  //Compute back based on front (only once)
  if (ToCompute(texPos)){
    bool alive = GetAt(texPos);
    int neighborCount = NeighborCount(texPos);

    if (BSNOTATION){
      //Change alive
      if (alive){ //Survival
        alive = false;
        for (int i = 0; i < SURVIVALCOUNT; i++){
          if (neighborCount == SURVIVAL[i]){
            alive = true;
            break;
          }
        }
      }else{ //Birth
        for (int i = 0; i < BIRTHCOUNT; i++){
          if (neighborCount == BIRTH[i]){
            alive = true;
            break;
          }
        }
      }
    }else{
      if (alive && (SURVIVAL[0] > neighborCount || SURVIVAL[1] < neighborCount)) alive = false;
      else if (!alive && BIRTH[0] <= neighborCount && BIRTH[1] >= neighborCount) alive = true;
    }

    if (alive) Alive(texPos);
    else       Dead(texPos);
  }
}
