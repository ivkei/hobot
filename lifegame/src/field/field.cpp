#include"field.h"

#include<random>

#include"hobot.h"

bool Field::GetAt(int x, int y) const{
  HT_LOG_ASSERT(x < _field.size() && y < _field[0].size(), "Accessing element outside the field");
  return _field[x][y];
}

//Cell size is between 0 and 1
Field::Field(hobot::Vec2 dimensions, hobot::Vec4 colorAlive, hobot::Vec2 cellSize, hobot::Vec4 gapColor, hobot::Vec4 colorDead, int population)
:_colorAlive(colorAlive), _cellSize(cellSize), _gapColor(gapColor), _colorDead(colorDead){
  for (int i = 0; i < dimensions.x; i++){
    _field.emplace_back();
    for (int j = 0; j < dimensions.y; j++){
      _field[i].push_back(population > rand() % 100);
    }
  }
}

//Population is percentage of a cell going alive
void Field::PopulateRandomly(int population){
  for (int i = 0; i < _field.size(); i++){
    for (int j = 0; j < _field[0].size(); j++){
      _field[i][j] = population > rand() % 100;
    }
  }
}
