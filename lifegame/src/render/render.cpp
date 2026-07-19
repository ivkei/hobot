#include"render.h"

void Render(const hobot::Renderer& renderer, const Field& field){
  renderer.Clear(field.GetColorGap());

  for (int i = 0; i < field.Width(); i++){
    for (int j = 0; j < field.Height(); j++){
      glm::vec2 fullDimensions = {2.0f/(float)field.Width(), 2.0f/(float)field.Height()};
      glm::vec2 dimensions = {fullDimensions.x*field.GetCellSize().x, fullDimensions.y*field.GetCellSize().y};

      glm::vec2 pos = {2.0f*((float)i/field.Width())-1.0f, 2.0f*((float)j/field.Height())-1.0f};
      pos.x += (fullDimensions.x - dimensions.x)/2.0f; //Account for dimensions
      pos.y += (fullDimensions.y - dimensions.y)/2.0f;

      if (field.GetAt(i, j)){
        renderer.Quad(pos, dimensions, field.GetColorAlive());
      }else{
        renderer.Quad(pos, dimensions, field.GetColorDead());
      }
    }
  }

  renderer.Render();
}
