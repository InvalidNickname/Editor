#ifndef MGD_UI_SRC_GUI_H
#define MGD_UI_SRC_GUI_H

#include <map>

#include <string>

#include "ui_drawable.h"

using namespace std;

class GUI {
 public:
  void Render(RenderWindow *window);

  bool CheckClicked(Vector2f coords);

  void AddObject(const string &name, UIDrawable *drawable);

  void DeleteObject(const string &name);

  [[nodiscard]] UIDrawable *Get(const string &name) const;

 private:
  map<string, UIDrawable *> drawables_;
};

#endif