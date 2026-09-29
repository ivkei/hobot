#pragma once

#include"ht_math/math.h"
#include"ht_file/file.h"

namespace hobot{
//TODO: try polymorphism (TOOOOOOOOOOOOOOOOO LAGGY (probably))

struct VertexData{
  hobot::Vec2 pos;
  hobot::Vec4 col;
};

struct Quad{
  Quad(hobot::Vec2 pos, hobot::Vec2 dim, hobot::Vec4 color = hobot::Vec4(1));

  hobot::Vec2 pos0;
  hobot::Vec2 pos1;
  hobot::Vec2 pos2;
  hobot::Vec2 pos3;

  hobot::Vec4 col0;
  hobot::Vec4 col1;
  hobot::Vec4 col2;
  hobot::Vec4 col3;

  std::vector<VertexData> VertexData();
};

struct Trig{
  hobot::Vec2 pos0;
  hobot::Vec2 pos1;
  hobot::Vec2 pos2;

  hobot::Vec4 col0;
  hobot::Vec4 col1;
  hobot::Vec4 col2;

  std::vector<VertexData> VertexData();
};

//Regular polygon
struct Reg{
  hobot::Vec2 cent;
  float inrad;
  int nVert;
  hobot::Vec4 centCol;
  hobot::Vec4 circCol;
  float rotation;
  //TODO: into pallete?

  std::vector<VertexData> VertexData();
};

}
