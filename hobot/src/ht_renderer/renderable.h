#pragma once

#include"ht_math/math.h"
#include"ht_file/file.h"

namespace hobot{

//TODO: ctors

/*TODO
* Either polymorphism with GetVertexData in each and simple renderer implementation with glMapBuffers
* Or, to avoid polymorphism, can just accept the vertex data directly (will eliminate need for raw)
* Consider the sparky approach
*/
struct Quad{
  hobot::Vec2 pos0;
  hobot::Vec2 pos1;
  hobot::Vec2 pos2;
  hobot::Vec2 pos3;

  hobot::Vec4 col0;
  hobot::Vec4 col1;
  hobot::Vec4 col2;
  hobot::Vec4 col3;
};

struct Trig{
  hobot::Vec2 pos0;
  hobot::Vec2 pos1;
  hobot::Vec2 pos2;

  hobot::Vec4 col0;
  hobot::Vec4 col1;
  hobot::Vec4 col2;
};

struct Reg{
  hobot::Vec2 cent;
  float inrad;
  int nVert;
  hobot::Vec4 centCol;
  hobot::Vec4 circCol;
  //TODO: into pallete?
};

}
