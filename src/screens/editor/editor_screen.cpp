#include "editor_screen.h"

void EditorScreen::Prepare() {
  // размеры экрана
  window_size_ = window_->getSize();
  // начальные размеры экрана
  initial_size_ = window_size_ / 2u;
  // установка камеры
  ui_.setSize((float) initial_size_.x, (float) initial_size_.y);
  ui_.setCenter(ui_.getSize() / 2.f);
  // создание GUI
  SetGUI();
  // создание холста
  for (int i = 0; i < WIDTH * HEIGHT * 4; i++) {
    pixels_[i] = 255;
  }
  texture_.create(WIDTH, HEIGHT);
  texture_.update(pixels_);
  canvas_.setTexture(texture_);
  // установка view для интерфейса
  ui_.setSize((Vector2f) window_size_);
  ui_.setCenter((Vector2f) initial_size_);
}

void EditorScreen::SetGUI() {
  gui_ = new GUI();
}

AppState EditorScreen::DoAction() {
  HandleInput();
  UpdateTexture();
  return temp_state_;
}

void EditorScreen::HandleInput() {
  Event event{};
  while (window_->pollEvent(event)) {
    if (event.type == Event::MouseButtonPressed) {
      if (Mouse::isButtonPressed(Mouse::Left)) {
        Vector2i pos = Mouse::getPosition();
        if (gui_->CheckClicked((Vector2f) pos));
        else {
          if (active_vector_ == -1 || points_[active_vector_].size() == 4) {
            points_.emplace_back();
            active_vector_ = points_.size() - 1;
          }
          points_[active_vector_].emplace_back(pos.x, pos.y);
        }
      }
    }
    if (event.type == Event::Closed) {
      temp_state_ = EXIT;
    }
  }
}

void EditorScreen::Draw() {
  window_->setView(ui_);
  window_->draw(canvas_);
  DrawHelpers();
  gui_->Render(window_);
}

void EditorScreen::UpdateTexture() {
  // очистка экрана
  for (int i = 0; i < WIDTH * HEIGHT * 4; i++) {
    pixels_[i] = 255;
  }
  // отрисовка кривых
  for (const auto &i:points_) {
    DrawCurve(i);
  }
  texture_.update(pixels_);
}

void EditorScreen::DrawCurve(const vector<pair<float, float>> &points) {
  if (points.size() == 4) {
    for (float t = 0; t <= 1; t += 0.0001) {
      float cx = 3.f * (points[1].first - points[0].first);
      float bx = 3.f * (points[2].first - points[1].first) - cx;
      float ax = points[3].first - points[0].first - bx - cx;

      float cy = 3.f * (points[1].second - points[0].second);
      float by = 3.f * (points[2].second - points[1].second) - cy;
      float ay = points[3].second - points[0].second - by - cy;

      int x = (int) (ax * pow(t, 3) + bx * pow(t, 2) + cx * t + points[0].first);
      int y = (int) (ay * pow(t, 3) + by * pow(t, 2) + cy * t + points[0].second);

      pixels_[(x + y * WIDTH) * 4] = 0;
      pixels_[(x + y * WIDTH) * 4 + 1] = 0;
      pixels_[(x + y * WIDTH) * 4 + 2] = 0;
    }
  }
}

void EditorScreen::DrawHelpers() {
  if (active_vector_ != -1) {
    uint32_t size = points_[active_vector_].size();
    // отрисовка активных точек
    vector<RectangleShape> main_points_{size, RectangleShape({6.f, 6.f})};
    for (int i = 0; i < size; ++i) {
      main_points_[i].setFillColor(GRAY);
      main_points_[i].setOrigin(3, 3);
      main_points_[i].setPosition(points_[active_vector_][i].first, points_[active_vector_][i].second);
    }
    for (const auto &i : main_points_) {
      window_->draw(i);
    }
    // отрисовка линий к 2 и 3 опорным точкам
    if (size >= 2) {
      Vertex line[2];
      line[0].position = Vector2f(points_[active_vector_][0].first, points_[active_vector_][0].second);
      line[0].color = GRAY;
      line[1].position = Vector2f(points_[active_vector_][1].first, points_[active_vector_][1].second);
      line[1].color = GRAY;
      window_->draw(line, 2, Lines);
    }
    if (size == 4) {
      Vertex line[2];
      line[0].position = Vector2f(points_[active_vector_][2].first, points_[active_vector_][2].second);
      line[0].color = GRAY;
      line[1].position = Vector2f(points_[active_vector_][3].first, points_[active_vector_][3].second);
      line[1].color = GRAY;
      window_->draw(line, 2, Lines);
    }
  }
}
