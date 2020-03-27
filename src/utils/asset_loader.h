#ifndef MGD_SRC_UTILS_ASSET_LOADER_H
#define MGD_SRC_UTILS_ASSET_LOADER_H

#include <map>
#include <string>

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class AssetLoader {
 public:
  static AssetLoader &Get();

  AssetLoader(AssetLoader const &) = delete;

  AssetLoader &operator=(AssetLoader const &) = delete;

  [[nodiscard]] Texture *GetTexture(const string &name) const;

  [[nodiscard]] Font *GetFont(const string &name) const;

 private:
  map<string, Texture *> texture_map_;
  map<string, Font *> font_map_;

  AssetLoader();

  ~AssetLoader() = default;

  void LoadTexture(const string &pathname, const string &name, IntRect position = IntRect());

  void LoadFont(const string &pathname, const string &name);
};

#endif