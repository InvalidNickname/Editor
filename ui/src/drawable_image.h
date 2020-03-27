#ifndef MGD_UI_SRC_DRAWABLE_IMAGE_H
#define MGD_UI_SRC_DRAWABLE_IMAGE_H

#include "ui_drawable.h"

class DrawableImage : public UIDrawable {
 public:
  DrawableImage(Vector2f position, Vector2f size, Texture *texture);

  void Render(RenderWindow *window) override;

  [[nodiscard]] bool CheckClicked(Vector2f coords) override { return false; };

  [[nodiscard]] bool IsClickable() override { return false; };

 private:
  Sprite sprite_;
};

#endif