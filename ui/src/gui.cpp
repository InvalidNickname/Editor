#include "gui.h"

void GUI::Render(RenderWindow *window) {
  for (auto &i : drawables_) {
    i.second->Render(window);
  }
}

bool GUI::CheckClicked(Vector2f coords) {
  for (auto &i : drawables_) {
    if (i.second->CheckClicked(coords)) {
      return true;
    }
  }
  return false;
}

void GUI::AddObject(const string &name, UIDrawable *drawable) {
  drawables_.insert(pair(name, drawable));
}

UIDrawable *GUI::Get(const string &name) const {
  return drawables_.at(name);
}
