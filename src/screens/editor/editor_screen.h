#ifndef MGD_SRC_SCREENS_MAP_MAP_SCREEN_H
#define MGD_SRC_SCREENS_MAP_MAP_SCREEN_H

#include <gui.h>
#include <radio_buttons.h>
#include <drawable_text.h>

#include "screens/screen.h"

class EditorScreen : public Screen {

  using Screen::Screen;

 private:
  const int WIDTH = 1920;
  const int HEIGHT = 1080;
  const Color GRAY{128, 128, 128};

  Vector2u window_size_, initial_size_;
  View ui_;
  GUI *gui_;
  Sprite canvas_{};
  Texture texture_;
  Uint8 *pixels_ = new Uint8[WIDTH * HEIGHT * 4];
  vector<vector<pair<float, float>>> points_;
  int active_vector_{-1};

  void Draw() override;

  AppState DoAction() override;

  void Prepare() override;

  void SetGUI();

  void HandleInput();

  void UpdateTexture();

  void DrawCurve(const vector<pair<float, float>> &points);

  void DrawHelpers();
};

#endif