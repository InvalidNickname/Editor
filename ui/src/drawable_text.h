#ifndef MGD_UI_SRC_DRAWABLE_TEXT_H
#define MGD_UI_SRC_DRAWABLE_TEXT_H

#include <codecvt>

#include "ui_drawable.h"

class DrawableText : public UIDrawable {
 public:
  DrawableText(Vector2f position, const std::string &text, uint8_t charSize, Font *font, Color color);

  void Render(RenderWindow *window) override;

  [[nodiscard]] bool CheckClicked(Vector2f coords) override { return false; };

  [[nodiscard]] bool IsClickable() override { return false; };

  void SetText(const std::string &text);

 private:
  Text text_;
};

#endif