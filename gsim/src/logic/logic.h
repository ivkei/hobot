#pragma once

#include"object/object.h"

#include<functional>

class Logic{
private:
  float _gravity = 1;
  std::function<hobot::Vec2(hobot::Vec2)> _wrapFunc;
public:
  Logic(std::function<hobot::Vec2(hobot::Vec2)> wrapFunc);
  //Correction is a scale factor of all quantities (ex: fixedUpdate can be passed so that physics can be done with same rate even though called at different times)
  void Execute(std::vector<Object>& objects, float correction);

  void SetGravity(float gravity) {_gravity = gravity;};
};

