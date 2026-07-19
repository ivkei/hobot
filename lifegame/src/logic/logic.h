#pragma once

#include"field/field.h"

class Logic{
private:
public:
  //How often logic should execute
  static float GetSecondsInterval();
  void Execute(Field& field);
};
