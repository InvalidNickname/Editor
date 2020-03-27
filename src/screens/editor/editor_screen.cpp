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
  canvas_.setPosition(32, 0);
  // установка view для интерфейса
  ui_.setSize((Vector2f) window_size_);
  ui_.setCenter((Vector2f) initial_size_);
}

void EditorScreen::SetGUI() {
  gui_ = new GUI();
  gui_->AddObject("btn_select", new Button(
      Vector2f(0, 0),
      Vector2f(32, 32),
      AssetLoader::Get().GetTexture("btn_temp"),
      AssetLoader::Get().GetTexture("btn_temp"),
      [this]() { cur_state_ = SELECT; }
  ));
  gui_->AddObject("btn_new", new Button(
      Vector2f(0, 32),
      Vector2f(32, 32),
      AssetLoader::Get().GetTexture("btn_temp"),
      AssetLoader::Get().GetTexture("btn_temp"),
      [this]() { cur_state_ = NEW; }
  ));
  gui_->AddObject("btn_delete", new Button(
      Vector2f(0, 64),
      Vector2f(32, 32),
      AssetLoader::Get().GetTexture("btn_temp"),
      AssetLoader::Get().GetTexture("btn_temp"),
      [this]() { cur_state_ = DELETE; }
  ));
  gui_->AddObject("btn_edit", new Button(
      Vector2f(0, 96),
      Vector2f(32, 32),
      AssetLoader::Get().GetTexture("btn_temp"),
      AssetLoader::Get().GetTexture("btn_temp"),
      [this]() { cur_state_ = EDIT; }
  ));
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
        if (gui_->CheckClicked(Vector2f(pos.x, pos.y)));
        else if (pos.x > 32) {
          switch (cur_state_) {
            case NEW:
              if (active_vector_ == -1) {
                points_.emplace_back();
                active_vector_ = points_.size() - 1;
              }
              points_[active_vector_].emplace_back(pos.x - 100, pos.y); // направляющая 1
              points_[active_vector_].emplace_back(pos.x, pos.y); // опорная
              points_[active_vector_].emplace_back(pos.x + 100, pos.y); // направляющая 2
              break;
            case EDIT:
              if (active_vector_ != -1) {
                selected_point_ = IsOnPoint(pos, points_[active_vector_]);
                if (selected_point_ != -1) cur_state_ = EDITING;
              }
              break;
            case EDITING:
              if (selected_point_ % 3 == 1) {
                //выбрана опорная точка
                float dx = points_[active_vector_][selected_point_].first - pos.x;
                float dy = points_[active_vector_][selected_point_].second - pos.y;
                points_[active_vector_][selected_point_ - 1].first -= dx;
                points_[active_vector_][selected_point_ - 1].second -= dy;
                points_[active_vector_][selected_point_ + 1].first -= dx;
                points_[active_vector_][selected_point_ + 1].second -= dy;
                points_[active_vector_][selected_point_].first = pos.x;
                points_[active_vector_][selected_point_].second = pos.y;
              } else if (selected_point_ % 3 == 0) {
                // выбрана 1 направляющая
                float dx = points_[active_vector_][selected_point_].first - pos.x;
                float dy = points_[active_vector_][selected_point_].second - pos.y;
                points_[active_vector_][selected_point_ + 2].first += dx;
                points_[active_vector_][selected_point_ + 2].second += dy;
                points_[active_vector_][selected_point_].first = pos.x;
                points_[active_vector_][selected_point_].second = pos.y;
              } else {
                float dx = points_[active_vector_][selected_point_].first - pos.x;
                float dy = points_[active_vector_][selected_point_].second - pos.y;
                points_[active_vector_][selected_point_ - 2].first += dx;
                points_[active_vector_][selected_point_ - 2].second += dy;
                points_[active_vector_][selected_point_].first = pos.x;
                points_[active_vector_][selected_point_].second = pos.y;
              }
              break;
            case DELETE:
              break;
            case SELECT:
              bool found{false};
              for (int i = 0; i < points_.size(); ++i) {
                if (IsOnCurve(pos, points_[i])) {
                  active_vector_ = i;
                  found = true;
                  break;
                }
              }
              if (!found) active_vector_ = -1;
              break;
          }
        }
      } else if (Mouse::isButtonPressed(Mouse::Right)) {
        active_vector_ = -1;
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
  for (int i = 1; i < points.size(); i += 3) {
    if (points.size() - i < 4) break;
    for (size_t j = 0; j <= 10000; ++j) {
      float t = j / 10000.f;

      float cx = 3.f * (points[i + 1].first - points[i].first);
      float bx = 3.f * (points[i + 2].first - points[i + 1].first) - cx;
      float ax = points[i + 3].first - points[i].first - bx - cx;

      float cy = 3.f * (points[i + 1].second - points[i].second);
      float by = 3.f * (points[i + 2].second - points[i + 1].second) - cy;
      float ay = points[i + 3].second - points[i].second - by - cy;

      int x = (int) (ax * pow(t, 3) + bx * pow(t, 2) + cx * t + points[i].first) - 32;
      int y = (int) (ay * pow(t, 3) + by * pow(t, 2) + cy * t + points[i].second);

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
    // отрисовка линий от опорных точек к направляющим
    for (int i = 0; i < size; i += 3) {
      Vertex line[3];
      line[0].position = Vector2f(points_[active_vector_][i].first, points_[active_vector_][i].second);
      line[0].color = GRAY;
      line[1].position = Vector2f(points_[active_vector_][i + 1].first, points_[active_vector_][i + 1].second);
      line[1].color = GRAY;
      line[2].position = Vector2f(points_[active_vector_][i + 2].first, points_[active_vector_][i + 2].second);
      line[2].color = GRAY;
      window_->draw(line, 3, LineStrip);
    }
  }
}

bool EditorScreen::IsOnCurve(const Vector2i &pos, const vector<pair<float, float>> &points) {
  for (int i = 1; i < points.size(); i += 3) {
    if (points.size() - i < 4) break;
    for (size_t j = 0; j <= 1000; ++j) {
      float t = j / 1000.f;

      float cx = 3.f * (points[i + 1].first - points[i].first);
      float bx = 3.f * (points[i + 2].first - points[i + 1].first) - cx;
      float ax = points[i + 3].first - points[i].first - bx - cx;

      float cy = 3.f * (points[i + 1].second - points[i].second);
      float by = 3.f * (points[i + 2].second - points[i + 1].second) - cy;
      float ay = points[i + 3].second - points[i].second - by - cy;

      int x = (int) (ax * pow(t, 3) + bx * pow(t, 2) + cx * t + points[i].first);
      int y = (int) (ay * pow(t, 3) + by * pow(t, 2) + cy * t + points[i].second);

      double dst = sqrt(pow(x - pos.x, 2) + pow(y - pos.y, 2));
      if (dst < 10) {
        return true;
      }
    }
  }
  return false;
}

int EditorScreen::IsOnPoint(const Vector2i &pos, const vector<pair<float, float>> &points) {
  for (int i = 0; i < points.size(); ++i) {
    double dst = sqrt(pow(points[i].first - pos.x, 2) + pow(points[i].second - pos.y, 2));
    if (dst < 10) {
      return i;
    }
  }
  return -1;
}
