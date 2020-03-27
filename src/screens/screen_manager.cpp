#include "screen_manager.h"

ScreenManager::ScreenManager(AppState initial_key, uint32_t length) : state_(initial_key) {
  screens_ = std::vector<Screen *>(length);
}

void ScreenManager::AddScreen(AppState state, Screen *screen) {
  screens_[state] = screen;
}

bool ScreenManager::DoActiveScreenActions() {
  if (state_ >= 0) {
    // если это новый экран - подготовить
    if (state_ != prev_state_) {
      screens_[state_]->Prepare();
      prev_state_ = state_;
    }
    temp_state_ = screens_[state_]->DoAction();
    if (temp_state_ != THIS_STATE) {
      state_ = temp_state_;
    }
    return true;
  }
  return false;
}

void ScreenManager::DrawActiveScreen() {
  // если этот экран подоготовлен - отрисовка
  if (state_ >= 0 && state_ == prev_state_) {
    screens_[state_]->Draw();
  }
}