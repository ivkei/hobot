#include"logic.h"

Logic::Logic(std::function<hobot::Vec2(hobot::Vec2)> wrapFunc) : _wrapFunc(wrapFunc){
}

void Logic::Execute(std::vector<Object>& objects, float correction){
  //Gravity
  for (int i = 0; i < objects.size(); i++){
    for (int j = 0; j < objects.size(); j++){
      if (i == j) continue;

      hobot::Vec2 dment = objects[j].pos - objects[i].pos;
      float iD = 1.0f/(hobot::Abs(dment)+1e-10); //Avoid division by 0

      objects[i].vel += iD*_gravity*hobot::Norm(dment)*0.015f*objects[j].mass;
    }
  }

  //Velocities applied
  for (auto&& object : objects){
    object.pos += object.vel*correction;

    //Wrap position
    object.pos = _wrapFunc(object.pos);
  }
}
