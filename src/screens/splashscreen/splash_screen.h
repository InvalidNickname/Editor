#ifndef MGD_SRC_SCREENS_SPLASHSCREEN_SPLASH_SCREEN_H
#define MGD_SRC_SCREENS_SPLASHSCREEN_SPLASH_SCREEN_H

#include <future>
#include <chrono>

#include "screens/screen.h"
#include "utils/asset_loader.h"
#include "utils/r.h"

class SplashScreen : public Screen {

  using Screen::Screen;

 private:
  std::future<void> loading_thread_;

  void Prepare() override;

  void Load();

  AppState DoAction() override;

  void Draw() override;
};

#endif