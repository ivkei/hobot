#include"ht_pch/pch.h"

#include"renderable.h"

namespace hobot{

Quad::Quad(hobot::Vec2 pos, hobot::Vec2 dim, hobot::Vec4 color)
: pos0(pos), pos1({pos.x+dim.x, pos.y}), pos2({pos.x, pos.y+dim.y}), pos3(pos+dim),
  col0(color), col1(color), col2(color), col3(color){
}

std::vector<VertexData> Quad::VertexData(){
  std::vector<class VertexData> data;

  data.emplace_back(pos0, col0);
  data.emplace_back(pos1, col1);
  data.emplace_back(pos2, col2);
  data.emplace_back(pos3, col3);

  return data;
}

std::vector<VertexData> Trig::VertexData(){
  std::vector<class VertexData> data;

  data.emplace_back(pos0, col0);
  data.emplace_back(pos1, col1);
  data.emplace_back(pos2, col2);

  return data;
}

std::vector<VertexData> Reg::VertexData(){

  std::vector<class VertexData> data;

  for (int i = 0; i < nVert; i++){
    float angle1 = ((float)i/(float)nVert)*2.0f*PI<float>() + (rotation);
    float angle2 = ((float)(i+1)/(float)nVert)*2.0f*PI<float>() + (rotation);
    hobot::Vec2 p1{cent.x+std::cos(angle1)*inrad,cent.y+std::sin(angle1)*inrad}, p2{cent.x+std::cos(angle2)*inrad,cent.y+std::sin(angle2)*inrad};

    data.emplace_back(cent, centCol);
    data.emplace_back(p1, circCol);
    data.emplace_back(p2, circCol);
  }

  return data;

}

}
