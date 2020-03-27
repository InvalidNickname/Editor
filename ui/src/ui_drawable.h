#ifndef MGD_UI_SRC_UI_DRAWABLE_H
#define MGD_UI_SRC_UI_DRAWABLE_H

#include <SFML/Graphics.hpp>

using namespace sf;

using Vector2s = Vector2<int16_t>;

class UIDrawable {
 public:
  virtual void Render(RenderWindow *window) = 0;

  [[nodiscard]] virtual bool CheckClicked(Vector2f coords) = 0;

  [[nodiscard]] virtual bool IsClickable() = 0;
};

#endif