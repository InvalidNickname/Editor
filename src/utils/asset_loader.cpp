#include "asset_loader.h"

AssetLoader &AssetLoader::Get() {
  static AssetLoader s;
  return s;
}

AssetLoader::AssetLoader() {

}

Texture *AssetLoader::GetTexture(const string &name) const {
  return texture_map_.at(name);
}

void AssetLoader::LoadTexture(const string &pathname, const string &name, IntRect position) {
  auto texture = new Texture();
  if (texture->loadFromFile("graphics/" + pathname, position)) {
    texture->setSmooth(true);
    texture_map_.insert(pair(name, texture));
  }
}

Font *AssetLoader::GetFont(const string &name) const {
  return font_map_.at(name);
}

void AssetLoader::LoadFont(const string &pathname, const string &name) {
  auto font = new Font();
  if (font->loadFromFile("fonts/" + pathname)) {
    font_map_.insert(pair(name, font));
  }
}
