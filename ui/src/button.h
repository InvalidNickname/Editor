#ifndef MGD_UI_SRC_BUTTON_H
#define MGD_UI_SRC_BUTTON_H

#include <functional>
#include <utility>

#include <SFML/Graphics.hpp>

#include "ui_drawable.h"

using namespace std;

class Button : public UIDrawable {

 public:
  Button(Vector2f position, Vector2f size, Texture *normal, Texture *clicked, function<void()> onClick);

  void Render(RenderWindow *window) override;

  bool CheckClicked(Vector2f coords) override;

  void SetClicked(bool clicked);

  bool IsClickable() override { return true; }

 private:
  Sprite sprite_;
  Texture *normal_, *clicked_;
  const function<void()> onClick_;
};

#endif