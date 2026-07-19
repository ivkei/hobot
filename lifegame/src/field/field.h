#pragma once

#include<vector>

#include"glm/vec2.hpp"
#include"glm/vec4.hpp"

class Field{
private:
  std::vector<std::vector<bool>> _field;
  glm::vec4 _colorAlive;
  glm::vec2 _cellSize;
  glm::vec4 _gapColor;
  glm::vec4 _colorDead;
public:
  //Cell size is between 0 and 1
  Field(glm::vec2 dimensions, glm::vec4 colorAlive, glm::vec2 cellSize, glm::vec4 gapColor, glm::vec4 colorDead, int population = 0);

  bool GetAt(int x, int y) const;
  int Width() const {return _field.size();};
  int Height() const {return _field[0].size();};
  glm::vec4 GetColorAlive() const { return _colorAlive; };
  glm::vec4 GetColorDead() const { return _colorDead; };
  glm::vec4 GetColorGap() const { return _gapColor; };
  //Again, between 0 and 1
  glm::vec2 GetCellSize() const { return _cellSize; };
  std::vector<std::vector<bool>>& GetField() { return _field; }
  

  //Population is percentage of a cell going alive
  void PopulateRandomly(int population);
};
