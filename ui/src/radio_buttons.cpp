#include "radio_buttons.h"

RadioButtons::RadioButtons(map<string, Button *> *buttons, const string &default_clicked) : buttons_(buttons) {
  SetClicked(default_clicked);
}

void RadioButtons::Render(RenderWindow *window) {
  for (auto &i : *buttons_) {
    i.second->Render(window);
  }
}

bool RadioButtons::CheckClicked(Vector2f coords) {
  bool check = false;
  for (auto &i : *buttons_) {
    if (i.second->CheckClicked(coords)) {
      clicked_ = i.first;
      check = true;
      break;
    }
  }
  if (!clicked_.empty()) SetClicked(clicked_);
  return check;
}

void RadioButtons::SetClicked(const string &name) {
  clicked_ = name;
  for (auto &i : *buttons_) {
    i.second->SetClicked(false);
  }
  buttons_->at(clicked_)->SetClicked(true);
}

map<string, Button *> *RadioButtons::GetMap() {
  return buttons_;
}

RadioButtons::RadioButtons() {
  buttons_ = new map<string, Button *>();
}
