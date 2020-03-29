#ifndef MGD_SRC_SCREENS_MAP_MAP_SCREEN_H
#define MGD_SRC_SCREENS_MAP_MAP_SCREEN_H

#include <gui.h>
#include <radio_buttons.h>
#include <drawable_text.h>

#include "screens/screen.h"
#include "editor_state.h"
#include "utils/asset_loader.h"

class EditorScreen : public Screen {

  using Screen::Screen;

 private:
  const Vector2u GUI_SIZE = Vector2u(32, 32);
  const Color GRAY{128, 128, 128};

  Vector2u window_size_, initial_size_, canvas_size_;
  View ui_;
  GUI *gui_;
  Sprite canvas_;
  Texture texture_;
  Uint8 *pixels_;
  vector<vector<Vector2f>> curves_;
  int cur_curve_{-1}, cur_point_{-1};
  EditorState cur_state_{ADD};
  bool editing_{false};

  void Draw() override;

  AppState DoAction() override;

  void Prepare() override;

  void SetGUI();

  void HandleInput();

  void UpdateTexture();

  void DrawCurve(const vector<Vector2f> &curve);

  void DrawHelpers();

  static bool IsCurveSelected(const Vector2i &pos, const vector<Vector2f> &curve);

  static int GetSelectedPoint(const Vector2i &pos, const vector<Vector2f> &curve);

  static void EditPoint(const Vector2f &to, int point, vector<Vector2f> &curve);

  static void RemovePoint(int &point, int &curve, vector<vector<Vector2f>> &curves);

  static int FindSelectedCurve(const Vector2i &pos, vector<vector<Vector2f>> &curves);

  static bool IsCyclic(const vector<Vector2f> &curve);
};

#endif