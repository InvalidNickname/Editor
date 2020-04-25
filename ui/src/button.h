#ifndef MGD_UI_SRC_BUTTON_H
#define MGD_UI_SRC_BUTTON_H

#include <functional>
#include <utility>

#include <SFML/Graphics.hpp>

#include "ui_drawable.h"

using namespace std;
using fvoid = function<void()>;

class Button : public UIDrawable {

 public:
  Button(Vector2f position, Vector2f size, Texture *normal, Texture *clicked, fvoid onClick);
  Button(Vector2f position, Vector2f size, Texture *normal, Texture *clicked, fvoid onClick, fvoid onNotClick);
  void Render(RenderWindow *window) override;

  bool CheckClicked(Vector2f coords) override;

  void SetClicked(bool clicked);

  bool IsClickable() override { return true; }

 private:
  Sprite sprite_;
  Texture *normal_, *clicked_;
  const function<void()> onClick_;
  const function<void()> onNotClick_;
};

#endif