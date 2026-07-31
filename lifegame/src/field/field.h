#pragma once

#include<vector>

#include"hobot.h"

class Field{
private:
  std::vector<std::vector<bool>> _field;
  hobot::Vec4 _colorAlive;
  hobot::Vec2 _cellSize;
  hobot::Vec4 _gapColor;
  hobot::Vec4 _colorDead;
public:
  //Cell size is between 0 and 1
  Field(hobot::Vec2 dimensions, hobot::Vec4 colorAlive, hobot::Vec2 cellSize, hobot::Vec4 gapColor, hobot::Vec4 colorDead, int population = 0);

  bool GetAt(int x, int y) const;
  int Width() const {return _field.size();};
  int Height() const {return _field[0].size();};
  hobot::Vec4 GetColorAlive() const { return _colorAlive; };
  hobot::Vec4 GetColorDead() const { return _colorDead; };
  hobot::Vec4 GetColorGap() const { return _gapColor; };
  //Again, between 0 and 1
  hobot::Vec2 GetCellSize() const { return _cellSize; };
  std::vector<std::vector<bool>>& GetField() { return _field; }
  

  //Population is percentage of a cell going alive
  void PopulateRandomly(int population);
};
