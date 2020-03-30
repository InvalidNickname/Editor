#include "asset_loader.h"

AssetLoader &AssetLoader::Get() {
  static AssetLoader s;
  return s;
}

AssetLoader::AssetLoader() {
  LoadTexture("btn_add.png", "btn_add_0", {0, 0, 64, 64});
  LoadTexture("btn_add.png", "btn_add_1", {64, 0, 64, 64});
  LoadTexture("btn_edit.png", "btn_edit_0", {0, 0, 64, 64});
  LoadTexture("btn_edit.png", "btn_edit_1", {64, 0, 64, 64});
  LoadTexture("btn_remove.png", "btn_remove_0", {0, 0, 64, 64});
  LoadTexture("btn_remove.png", "btn_remove_1", {64, 0, 64, 64});
  LoadTexture("btn_select.png", "btn_select_0", {0, 0, 64, 64});
  LoadTexture("btn_select.png", "btn_select_1", {64, 0, 64, 64});
  LoadTexture("btn_remove_2.png", "btn_remove_2_0", {0, 0, 64, 64});
  LoadTexture("btn_remove_2.png", "btn_remove_2_1", {64, 0, 64, 64});
  LoadTexture("btn_add_2.png", "btn_add_2_0", {0, 0, 64, 64});
  LoadTexture("btn_add_2.png", "btn_add_2_1", {64, 0, 64, 64});
  LoadTexture("btn_exit.png", "btn_exit_0", {0, 0, 64, 64});
  LoadTexture("btn_exit.png", "btn_exit_1", {64, 0, 64, 64});
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
