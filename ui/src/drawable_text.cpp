#include "drawable_text.h"

DrawableText::DrawableText(Vector2f position, const std::string &text, uint8_t charSize, Font *font, Color color) {
  text_.setFont(*font);
  text_.setCharacterSize(charSize);
  text_.setPosition(position.x, position.y);
  text_.setFillColor(color);
  SetText(text);
}

void DrawableText::Render(sf::RenderWindow *window) {
  window->draw(text_);
}

void DrawableText::SetText(const std::string &text) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  text_.setString(converter.from_bytes(text));
}