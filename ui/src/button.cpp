#include "button.h"

Button::Button(Vector2f position, Vector2f size, Texture *normal, Texture *clicked, function<void()> onClick)
    : normal_(normal), clicked_(clicked), onClick_(move(onClick)) {
  sprite_.setTexture(*normal);
  auto texture_size = normal->getSize();
  sprite_.setScale((float) size.x / texture_size.x, (float) size.y / texture_size.y);
  sprite_.setPosition(position.x, position.y);
}

Button::Button(Vector2f position, Vector2f size, Texture *normal, Texture *clicked, fvoid onClick, fvoid onNotClick)
    : normal_(normal), clicked_(clicked), onClick_(move(onClick)), onNotClick_(move(onNotClick)) {
  sprite_.setTexture(*normal);
  auto texture_size = normal->getSize();
  sprite_.setScale((float) size.x / texture_size.x, (float) size.y / texture_size.y);
  sprite_.setPosition(position.x, position.y);
}

void Button::Render(RenderWindow *window) {
  window->draw(sprite_);
}

bool Button::CheckClicked(Vector2f coords) {
  if (sprite_.getGlobalBounds().contains(coords.x, coords.y)) {
    SetClicked(true);
    return true;
  } else {
    SetClicked(false);
    return false;
  }
}

void Button::SetClicked(bool clicked) {
  if (clicked) {
    sprite_.setTexture(*clicked_);
    onClick_();
  } else {
    sprite_.setTexture(*normal_);
    if (onNotClick_ != nullptr) {
      onNotClick_();
    }
  }
}