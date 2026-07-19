#include"logic.h"

//How often logic should execute
float Logic::GetSecondsInterval(){
  return 0.1f;
}

static int NeighborCount(int x, int y, Field& field){
  int count = 0;
  for (int i = -1; i <= 1; i++){
    for (int j = -1; j <= 1; j++){
      int xPrime = ((x+i) + field.Width()) % field.Width();
      int yPrime = ((y+j) + field.Height()) % field.Height();
      if (x == xPrime && y == yPrime) continue;
      if (field.GetAt(xPrime, yPrime)) count++;
    }
  }
  return count;
}

void Logic::Execute(Field& field){
  std::vector<std::vector<bool>> newField;

  for (int i = 0; i < field.Width(); i++){
    newField.emplace_back();
    for (int j = 0; j < field.Height(); j++){
      bool alive = field.GetAt(i, j);
      int neighborCount = NeighborCount(i, j, field);

      if (alive && (neighborCount < 2 || neighborCount > 3)) alive = false;
      else if (!alive && neighborCount == 3) alive = true;

      newField[i].push_back(alive);
    }
  }

  field.GetField() = newField;
}
