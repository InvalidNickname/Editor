#ifndef MGD_SRC_SCREENS_SCREEN_MANAGER_H
#define MGD_SRC_SCREENS_SCREEN_MANAGER_H

#include "screen.h"

class ScreenManager {
 public:
  explicit ScreenManager(AppState initial_key, uint32_t length);

  void AddScreen(AppState state, Screen *screen);

  bool DoActiveScreenActions();

  void DrawActiveScreen();

 private:
  AppState state_, temp_state_{SPLASHSCREEN}, prev_state_{EXIT};

  std::vector<Screen *> screens_;
};

#endif