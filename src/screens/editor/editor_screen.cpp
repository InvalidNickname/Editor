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
  gui_->AddObject("menu", new RadioButtons(new map<string, Button *>{
      pair("btn_select", new Button(
          Vector2f(0, 0),
          Vector2f(32, 32),
          AssetLoader::Get().GetTexture("btn_select_0"),
          AssetLoader::Get().GetTexture("btn_select_1"),
          [this]() { cur_state_ = SELECT; }
      )),
      pair("btn_add", new Button(
          Vector2f(0, 32),
          Vector2f(32, 32),
          AssetLoader::Get().GetTexture("btn_add_0"),
          AssetLoader::Get().GetTexture("btn_add_1"),
          [this]() { cur_state_ = ADD; }
      )),
      pair("btn_remove", new Button(
          Vector2f(0, 64),
          Vector2f(32, 32),
          AssetLoader::Get().GetTexture("btn_remove_0"),
          AssetLoader::Get().GetTexture("btn_remove_1"),
          [this]() { cur_state_ = DELETE; }
      )),
      pair("btn_edit", new Button(
          Vector2f(0, 96),
          Vector2f(32, 32),
          AssetLoader::Get().GetTexture("btn_edit_0"),
          AssetLoader::Get().GetTexture("btn_edit_1"),
          [this]() { cur_state_ = EDIT; }
      ))
  }, "btn_select"));
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
        if (gui_->CheckClicked(Vector2f(pos.x, pos.y))) {

        } else if (pos.x > 32) {
          switch (cur_state_) {
            case ADD:
              if (active_vector_ == -1) {
                points_.emplace_back();
                active_vector_ = points_.size() - 1;
              }
              points_[active_vector_].emplace_back(pos.x - 50, pos.y); // направляющая 1
              points_[active_vector_].emplace_back(pos.x, pos.y); // опорная
              points_[active_vector_].emplace_back(pos.x + 50, pos.y); // направляющая 2
              break;
            case EDIT:
              if (active_vector_ != -1) {
                if (!editing) {
                  selected_point_ = IsOnPoint(pos, points_[active_vector_]);
                  if (selected_point_ != -1) editing = true;
                } else {
                  Edit(pos);
                  editing = false;
                }
              }
              break;
            case DELETE:
              Remove(pos);
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
  window_->clear(GRAY);
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

void EditorScreen::Edit(const Vector2i &pos) {
  float dx = points_[active_vector_][selected_point_].first - pos.x;
  float dy = points_[active_vector_][selected_point_].second - pos.y;
  points_[active_vector_][selected_point_] = {pos.x, pos.y};
  if (selected_point_ % 3 == 1) {
    //выбрана опорная точка
    points_[active_vector_][selected_point_ - 1].first -= dx;
    points_[active_vector_][selected_point_ - 1].second -= dy;
    points_[active_vector_][selected_point_ + 1].first -= dx;
    points_[active_vector_][selected_point_ + 1].second -= dy;
  } else if (selected_point_ % 3 == 0) {
    // выбрана 1 направляющая
    points_[active_vector_][selected_point_ + 2].first += dx;
    points_[active_vector_][selected_point_ + 2].second += dy;
  } else {
    // выбрана 2 направляющая
    points_[active_vector_][selected_point_ - 2].first += dx;
    points_[active_vector_][selected_point_ - 2].second += dy;
  }
}

void EditorScreen::Remove(const Vector2i &pos) {
  if (active_vector_ != -1) {
    selected_point_ = IsOnPoint(pos, points_[active_vector_]);
    if (selected_point_ != -1 && selected_point_ % 3 == 1) {
      if (selected_point_ == 1) {
        // обрезание левого края
        if (points_[active_vector_].size() == 6) {
          // остается одна точка из 2, удаляем обе
          points_.erase(points_.begin() + active_vector_);
          active_vector_ = -1;
        } else {
          points_[active_vector_].erase(points_[active_vector_].begin(),
                                        points_[active_vector_].begin() + 3);
        }
      } else if (selected_point_ == points_[active_vector_].size() - 2) {
        // обрезание правого края
        if (points_[active_vector_].size() == 6) {
          // остается одна точка из 2, удаляем обе
          points_.erase(points_.begin() + active_vector_);
          active_vector_ = -1;
        } else {
          points_[active_vector_].erase(points_[active_vector_].end() - 3,
                                        points_[active_vector_].end());
        }
      } else {
        // вырезание из центра, делится на 2 части
        if (points_[active_vector_].size() == 9) {
          // при вырезании центра из 3 точек, остаются 2 одиночные точки. удаляем их
          points_.erase(points_.begin() + active_vector_);
          active_vector_ = -1;
        } else {
          points_.emplace_back(points_[active_vector_].size() - selected_point_ - 2);
          std::copy(points_[active_vector_].begin() + selected_point_ + 2,
                    points_[active_vector_].end(),
                    points_.back().begin());
          points_[active_vector_].erase(points_[active_vector_].begin() + selected_point_ - 1,
                                        points_[active_vector_].end());
        }
      }
      selected_point_ = -1;
    }
  }
}
