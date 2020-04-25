#ifndef MGD_UI_SRC_RADIO_BUTTONS_H
#define MGD_UI_SRC_RADIO_BUTTONS_H

#include <map>
#include <string>

#include "button.h"

using namespace std;

class RadioButtons : public UIDrawable {
 public:
  RadioButtons(map<string, Button *> *buttons, const string &default_clicked);

  RadioButtons();

  void Render(RenderWindow *window) override;

  [[nodiscard]] bool CheckClicked(Vector2f coords) override;

  [[nodiscard]] bool IsClickable() override { return true; }

  void SetClicked(const string &name);

  map<string, Button *> *GetMap();

 private:
  map<string, Button *> *buttons_;
  string clicked_;
};

#endif