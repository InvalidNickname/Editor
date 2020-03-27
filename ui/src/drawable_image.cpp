#include "drawable_image.h"

DrawableImage::DrawableImage(Vector2f position, Vector2f size, Texture *texture) {
  sprite_.setTexture(*texture);
  auto texture_size = texture->getSize();
  sprite_.setScale(size.x / texture_size.x, size.y / texture_size.y);
  sprite_.setPosition(position.x, position.y);
}

void DrawableImage::Render(sf::RenderWindow *window) {
  window->draw(sprite_);
}
