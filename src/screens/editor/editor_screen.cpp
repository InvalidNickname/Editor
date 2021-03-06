#include <chrono>
#include "editor_screen.h"

void EditorScreen::Prepare() {
  // размеры экрана
  window_size_ = window_->getSize();
  // начальные размеры экрана
  initial_size_ = window_size_ / 2u;
  // установка камеры
  ui_.setSize(Vector2f(initial_size_.x, initial_size_.y));
  ui_.setCenter(ui_.getSize() / 2.f);
  // создание GUI
  SetGUI();
  // создание холста
  canvas_size_ = window_size_ - GUI_SIZE;
  pixels_ = new Uint8[canvas_size_.x * canvas_size_.y * 4];
  for (size_t i = 0; i < canvas_size_.x * canvas_size_.y * 4; i++) {
    pixels_[i] = 255;
  }
  texture_.create(canvas_size_.x, canvas_size_.y);
  texture_.update(pixels_);
  canvas_.setTexture(texture_);
  canvas_.setPosition(GUI_SIZE.x, GUI_SIZE.y);
  // установка view для интерфейса
  ui_.setSize((Vector2f) window_size_);
  ui_.setCenter((Vector2f) initial_size_);
}

void EditorScreen::SetGUI() {
  gui_ = new GUI();
  gui_->AddObject("menu", new RadioButtons(new map<string, Button *>{
      pair("btn_select", new Button(
          Vector2f(0, GUI_SIZE.y),
          Vector2f(32, 32),
          AssetLoader::Get().GetTexture("btn_select_0"),
          AssetLoader::Get().GetTexture("btn_select_1"),
          [this]() { cur_state_ = SELECT; }
      )),
      pair("btn_add", new Button(
          Vector2f(0, GUI_SIZE.y + 32),
          Vector2f(32, 32),
          AssetLoader::Get().GetTexture("btn_add_0"),
          AssetLoader::Get().GetTexture("btn_add_1"),
          [this]() { cur_state_ = ADD; }
      )),
      pair("btn_add_2", new Button(
          Vector2f(0, GUI_SIZE.y + 64),
          Vector2f(32, 32),
          AssetLoader::Get().GetTexture("btn_add_2_0"),
          AssetLoader::Get().GetTexture("btn_add_2_1"),
          [this]() { cur_state_ = ADD_2; }
      )),
      pair("btn_remove", new Button(
          Vector2f(0, GUI_SIZE.y + 96),
          Vector2f(32, 32),
          AssetLoader::Get().GetTexture("btn_remove_2_0"),
          AssetLoader::Get().GetTexture("btn_remove_2_1"),
          [this]() { cur_state_ = DELETE; }
      )),
      pair("btn_remove_2", new Button(
          Vector2f(0, GUI_SIZE.y + 128),
          Vector2f(32, 32),
          AssetLoader::Get().GetTexture("btn_remove_0"),
          AssetLoader::Get().GetTexture("btn_remove_1"),
          [this]() { cur_state_ = DELETE_2; }
      )),
      pair("btn_edit", new Button(
          Vector2f(0, GUI_SIZE.y + 160),
          Vector2f(32, 32),
          AssetLoader::Get().GetTexture("btn_edit_0"),
          AssetLoader::Get().GetTexture("btn_edit_1"),
          [this]() { cur_state_ = EDIT; }
      ))
  }, "btn_select"));
  gui_->AddObject("btn_exit", new Button(
      Vector2f(window_size_.x - 32, 0),
      Vector2f(32, 32),
      AssetLoader::Get().GetTexture("btn_exit_0"),
      AssetLoader::Get().GetTexture("btn_exit_1"),
      [this]() { temp_state_ = EXIT; }
  ));
  gui_->AddObject("btn_save", new Button(
      Vector2f(0, 0),
      Vector2f(32, 32),
      AssetLoader::Get().GetTexture("btn_save_0"),
      AssetLoader::Get().GetTexture("btn_save_1"),
      [this]() { Save(); }
  ));
  gui_->AddObject("btn_load", new Button(
      Vector2f(32, 0),
      Vector2f(32, 32),
      AssetLoader::Get().GetTexture("btn_load_0"),
      AssetLoader::Get().GetTexture("btn_load_1"),
      [this]() { Load(); }
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
        Vector2f posf(pos.x, pos.y);
        if (gui_->CheckClicked(posf)) {

        } else if (pos.x > GUI_SIZE.x && pos.y > GUI_SIZE.y) {
          if (cur_curve_ == -1 && cur_state_ == ADD) {
            curves_.push_back({Vector2f(posf.x - 50, posf.y), posf, Vector2f(posf.x + 50, posf.y)});
            cur_curve_ = curves_.size() - 1;
          } else if (cur_state_ == SELECT) {
            cur_curve_ = FindSelectedCurve(pos, curves_);
          } else if (cur_curve_ != -1) {
            vector<Vector2f> &curve = curves_[cur_curve_];
            switch (cur_state_) {
              case EDIT:
                if (!editing_) {
                  cur_point_ = GetSelectedPoint(pos, curve);
                  if (cur_point_ != -1) editing_ = true;
                } else {
                  if ((cur_point_ == 0 || cur_point_ == 1 || cur_point_ == 2) && IsCyclic(curve)) {
                    EditPoint(posf, curve.size() - 3 + cur_point_, curve);
                  }
                  EditPoint(posf, cur_point_, curve);
                  editing_ = false;
                }
                break;
              case DELETE:
                cur_point_ = GetSelectedPoint(pos, curve);
                if (cur_point_ % 3 != 1) break;
                if (IsCyclic(curve)) {
                  if (cur_point_ == 1) {
                    int tmp_point = curve.size() - 2;
                    RemovePoint(tmp_point, cur_curve_, curves_);
                  } else {
                    vector<Vector2f> tmp_curve;
                    tmp_curve.insert(tmp_curve.end(), curve.begin() + cur_point_ + 2, curve.end());
                    tmp_curve.insert(tmp_curve.end(), curve.begin() + 3, curve.begin() + cur_point_ - 1);
                    curve = tmp_curve;
                    break;
                  }
                }
                RemovePoint(cur_point_, cur_curve_, curves_);
                break;
              case DELETE_2:
                cur_point_ = GetSelectedPoint(pos, curve);
                if (cur_point_ == -1 || cur_point_ % 3 != 1) break;
                if (curve.size() == 6) {
                  // остается одна точка из 2, удаляем обе
                  curves_.erase(curves_.begin() + cur_curve_);
                  cur_curve_ = -1;
                } else {
                  if (cur_point_ == 1 && IsCyclic(curve)) {
                    curve.erase(curve.begin(), curve.begin() + 3);
                    curve.erase(curve.end() - 3, curve.end());
                    curve.insert(curve.end(), curve.begin(), curve.begin() + 3);
                  } else {
                    curve.erase(curve.begin() + cur_point_ - 1, curve.begin() + cur_point_ + 2);
                  }
                }
                break;
              case SELECT:
                break;
              case ADD_2:
                if (GetSelectedPoint(pos, curve) % 3 != 1) {
                  double min = DBL_MAX;
                  int min_i = -1; // новая точка лежит между min_i + 1 и min_i + 2
                  float min_t = 0;
                  for (size_t i = 1; i < curve.size(); i += 3) {
                    if (curve.size() - i < 4) break;
                    for (size_t j = 0; j <= 1000; ++j) {
                      float t = j / 1000.f;

                      Vector2f c = 3.f * (curve[i + 1] - curve[i]);
                      Vector2f b = 3.f * (curve[i + 2] - curve[i + 1]) - c;
                      Vector2f a = curve[i + 3] - curve[i] - b - c;

                      Vector2f res = a * pow(t, 3) + b * pow(t, 2) + c * t + curve[i];

                      double dst = sqrt(pow(res.x - pos.x, 2) + pow(res.y - pos.y, 2));
                      if (dst < min) {
                        min = dst;
                        min_i = i;
                        min_t = t;
                      }
                    }
                  }
                  if (min > 10) break;

                  Vector2f p00 = curve[min_i];
                  Vector2f p10 = curve[min_i + 1];
                  Vector2f p20 = curve[min_i + 2];
                  Vector2f p30 = curve[min_i + 3];

                  Vector2f p21 = p20 * (1 - min_t) + p30 * min_t;

                  Vector2f p11 = p10 * (1 - min_t) + p20 * min_t;
                  Vector2f p12 = p11 * (1 - min_t) + p21 * min_t;

                  Vector2f p01 = p00 * (1 - min_t) + p10 * min_t;
                  Vector2f p02 = p01 * (1 - min_t) + p11 * min_t;
                  Vector2f p03 = p02 * (1 - min_t) + p12 * min_t;

                  if (min_i + 1 == 2 && IsCyclic(curve)) {
                    EditPoint(p01, curve.size() - 1, curve);
                  }
                  EditPoint(p01, min_i + 1, curve);

                  if (min_i + 2 == curve.size() - 3 && IsCyclic(curve)) {
                    EditPoint(p21, 0, curve);
                  }
                  EditPoint(p21, min_i + 2, curve);

                  curve.insert(curve.begin() + min_i + 2, p02);
                  curve.insert(curve.begin() + min_i + 3, p03);
                  curve.insert(curve.begin() + min_i + 4, p12);
                }
                break;
              case ADD:
                if (IsCyclic(curve)) break;
                int tmp_curve = FindSelectedCurve(pos, curves_);
                if (tmp_curve == -1) {
                  // просто добавляем точку в конец выбранной кривой
                  curve.emplace_back(posf.x - 50, posf.y); // направляющая 1
                  curve.push_back(posf); // опорная
                  curve.emplace_back(posf.x + 50, posf.y); // направляющая 2
                } else {
                  int tmp_point = GetSelectedPoint(pos, curves_[tmp_curve]);
                  if (tmp_curve != cur_curve_) {
                    // соединения с другими кривыми
                    if (tmp_point == 1) {
                      if (IsCyclic(curves_[tmp_curve])) break;
                      // соединяем конец выбранной кривой с началом другой кривой
                      curve.insert(curve.end(), curves_[tmp_curve].begin(), curves_[tmp_curve].end());
                      curves_.erase(curves_.begin() + tmp_curve);
                      if (tmp_curve < cur_curve_) {
                        cur_curve_--;
                      }
                    } else if (tmp_point == curves_[tmp_curve].size() - 2) {
                      if (IsCyclic(curves_[tmp_curve])) break;
                      // соединяем конец выбранной кривой с концом другой
                      curve.insert(curve.end(), curves_[tmp_curve].rbegin(), curves_[tmp_curve].rend());
                      curves_.erase(curves_.begin() + tmp_curve);
                      if (tmp_curve < cur_curve_) {
                        cur_curve_--;
                      }
                    } else {
                      // просто добавляем точку в конец выбранной кривой
                      curve.emplace_back(posf.x - 50, posf.y); // направляющая 1
                      curve.push_back(posf); // опорная
                      curve.emplace_back(posf.x + 50, posf.y); // направляющая 2
                    }
                  } else {
                    // соединение с собой (зацикливание)
                    if (tmp_point == 1) {
                      curve.push_back(curve[0]); // направляющая 1
                      curve.push_back(curve[1]); // опорная
                      curve.emplace_back(curve[2]); // направляющая 2
                    } else {
                      // просто добавляем точку в конец выбранной кривой
                      curve.emplace_back(posf.x - 50, posf.y); // направляющая 1
                      curve.push_back(posf); // опорная
                      curve.emplace_back(posf.x + 50, posf.y); // направляющая 2
                    }
                  }
                }
                break;
            }
          }
        }
      } else if (Mouse::isButtonPressed(Mouse::Right)) {
        cur_curve_ = -1;
        editing_ = false;
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
  for (size_t i = 0; i < canvas_size_.x * canvas_size_.y * 4; i++) {
    pixels_[i] = 255;
  }
  // отрисовка кривых
  for (const auto &i:curves_) {
    DrawCurve(i);
  }
  texture_.update(pixels_);
}

void EditorScreen::DrawCurve(const vector<Vector2f> &curve) {
  for (size_t i = 1; i < curve.size(); i += 3) {
    if (curve.size() - i < 4) break;
    for (size_t j = 0; j <= 10000; ++j) {
      float t = j / 10000.f;

      Vector2f c = 3.f * (curve[i + 1] - curve[i]);
      Vector2f b = 3.f * (curve[i + 2] - curve[i + 1]) - c;
      Vector2f a = curve[i + 3] - curve[i] - b - c;

      Vector2f tmp_res = a * pow(t, 3) + b * pow(t, 2) + c * t + curve[i];
      Vector2i res(tmp_res.x - GUI_SIZE.x, tmp_res.y - GUI_SIZE.y);

      pixels_[(res.x + res.y * canvas_size_.x) * 4] = 0;
      pixels_[(res.x + res.y * canvas_size_.x) * 4 + 1] = 0;
      pixels_[(res.x + res.y * canvas_size_.x) * 4 + 2] = 0;
    }
  }
}

void EditorScreen::DrawHelpers() {
  if (cur_curve_ != -1) {
    size_t size = curves_[cur_curve_].size();
    // отрисовка активных точек
    vector<RectangleShape> main_points{size, RectangleShape({6.f, 6.f})};
    for (size_t i = 0; i < size; ++i) {
      main_points[i].setFillColor(GRAY);
      main_points[i].setOrigin(3, 3);
      main_points[i].setPosition(curves_[cur_curve_][i]);
    }
    for (const auto &i : main_points) {
      window_->draw(i);
    }
    // отрисовка линий от опорных точек к направляющим
    for (size_t i = 0; i < size; i += 3) {
      Vertex line[3];
      line[0].position = curves_[cur_curve_][i];
      line[0].color = GRAY;
      line[1].position = curves_[cur_curve_][i + 1];
      line[1].color = GRAY;
      line[2].position = curves_[cur_curve_][i + 2];
      line[2].color = GRAY;
      window_->draw(line, 3, LineStrip);
    }
  }
}

bool EditorScreen::IsCurveSelected(const Vector2i &pos, const vector<Vector2f> &curve) {
  for (size_t i = 1; i < curve.size(); i += 3) {
    if (curve.size() - i < 4) break;
    for (size_t j = 0; j <= 1000; ++j) {
      float t = j / 1000.f;

      Vector2f c = 3.f * (curve[i + 1] - curve[i]);
      Vector2f b = 3.f * (curve[i + 2] - curve[i + 1]) - c;
      Vector2f a = curve[i + 3] - curve[i] - b - c;

      Vector2f res = a * pow(t, 3) + b * pow(t, 2) + c * t + curve[i];

      double dst = sqrt(pow(res.x - pos.x, 2) + pow(res.y - pos.y, 2));
      if (dst < 10) {
        return true;
      }
    }
  }
  return false;
}

int EditorScreen::GetSelectedPoint(const Vector2i &pos, const vector<Vector2f> &curve) {
  for (size_t i = 0; i < curve.size(); ++i) {
    double dst = sqrt(pow(curve[i].x - pos.x, 2) + pow(curve[i].y - pos.y, 2));
    if (dst < 10) {
      return i;
    }
  }
  return -1;
}

void EditorScreen::EditPoint(const Vector2f &to, int point, vector<Vector2f> &curve) {
  Vector2f delta = curve[point] - to;
  curve[point] = to;
  if (point % 3 == 1) {
    //выбрана опорная точка
    curve[point - 1] -= delta;
    curve[point + 1] -= delta;
  } else if (point % 3 == 0) {
    // выбрана левая направляющая
    Vector2f l1 = curve[point] - curve[point + 1];
    float cos = l1.x / sqrt(l1.x * l1.x + l1.y * l1.y);
    float sin = l1.y / sqrt(l1.x * l1.x + l1.y * l1.y);
    Vector2f l2 = curve[point + 2] - curve[point + 1];
    curve[point + 2] = curve[point + 1];
    curve[point + 2].x -= sqrt(l2.x * l2.x + l2.y * l2.y) * cos;
    curve[point + 2].y -= sqrt(l2.x * l2.x + l2.y * l2.y) * sin;
  } else {
    // выбрана правая направляющая
    Vector2f l1 = curve[point] - curve[point - 1];
    float cos = l1.x / sqrt(l1.x * l1.x + l1.y * l1.y);
    float sin = l1.y / sqrt(l1.x * l1.x + l1.y * l1.y);
    Vector2f l2 = curve[point - 2] - curve[point - 1];
    curve[point - 2] = curve[point - 1];
    curve[point - 2].x -= sqrt(l2.x * l2.x + l2.y * l2.y) * cos;
    curve[point - 2].y -= sqrt(l2.x * l2.x + l2.y * l2.y) * sin;
  }
}

void EditorScreen::RemovePoint(int &point, int &curve, vector<vector<Vector2f>> &curves) {
  if (point == -1 || point % 3 != 1) return;
  if (point == 1) {
    // обрезание левого края
    if (curves[curve].size() == 6) {
      // остается одна точка из 2, удаляем обе
      curves.erase(curves.begin() + curve);
      curve = -1;
    } else {
      curves[curve].erase(curves[curve].begin(), curves[curve].begin() + 3);
    }
  } else if (point == curves[curve].size() - 2) {
    // обрезание правого края
    if (curves[curve].size() == 6) {
      // остается одна точка из 2, удаляем обе
      curves.erase(curves.begin() + curve);
      curve = -1;
    } else {
      curves[curve].erase(curves[curve].end() - 3, curves[curve].end());
    }
  } else {
    // вырезание из центра, делится на 2 части
    if (curves[curve].size() == 9) {
      // при вырезании центра из 3 точек остаются 2 одиночные точки. удаляем их
      curves.erase(curves.begin() + curve);
      curve = -1;
    } else {
      curves.emplace_back(curves[curve].size() - point - 2);
      std::copy(curves[curve].begin() + point + 2, curves[curve].end(), curves.back().begin());
      curves[curve].erase(curves[curve].begin() + point - 1, curves[curve].end());
    }
  }
  point = -1;
}

int EditorScreen::FindSelectedCurve(const Vector2i &pos, vector<vector<Vector2f>> &curves) {
  int cur_curve = -1;
  for (int i = 0; i < curves.size(); ++i) {
    if (IsCurveSelected(pos, curves[i])) {
      cur_curve = i;
      break;
    }
  }
  return cur_curve;
}

bool EditorScreen::IsCyclic(const vector<Vector2f> &curve) {
  int size = curve.size();
  return size != 3 && curve[2] == curve[size - 1] && curve[1] == curve[size - 2] && curve[0] == curve[size - 3];
}

void EditorScreen::Save() {
  ofstream out;
  long long time = chrono::system_clock::now().time_since_epoch().count();
  string filename = "saves/" + to_string(time) + ".txt";
  out.open(filename);
  if (out.is_open()) {
    for (auto &curve : curves_) {
      for (auto &point: curve) {
        out << to_string(point.x) << " " << to_string(point.y) << " ";
      }
      out << "\n";
    }
    out.close();
  }
}

void EditorScreen::Load() {
  string saves_dir = "saves";
  vector<string> files{};
  for (const auto &entry : filesystem::directory_iterator(saves_dir)) {
    if (entry.path().filename().string() != "dummy") files.push_back(entry.path().filename().string());
  }
  gui_->AddObject("load_menu_buttons", new RadioButtons());
  for (int i = 0; i < files.size(); ++i) {
    string filename = files[i];
    if (filename.length() > 21) {
      filename.resize(18);
      filename.append("...");
    }
    ((RadioButtons *) gui_->Get("load_menu_buttons"))->GetMap()->insert(pair(to_string(i), new Button(
        Vector2f(32, 32 + 25 * i),
        Vector2f(230, 25),
        AssetLoader::Get().GetTexture("menu_background"),
        AssetLoader::Get().GetTexture("menu_background"),
        [this, files, i] {
          LoadFile(files[i]);
          gui_->DeleteObject("load_menu_buttons");
          for (int j = 0; j < files.size(); ++j) {
            gui_->DeleteObject("load_menu_text_" + to_string(j));
          }
        },
        [this, files] {
          gui_->DeleteObject("load_menu_buttons");
          for (int j = 0; j < files.size(); ++j) {
            gui_->DeleteObject("load_menu_text_" + to_string(j));
          }
        }
    )));
    gui_->AddObject("load_menu_text_" + to_string(i), new DrawableText(
        Vector2f(35, 32 + 25 * i),
        filename,
        20,
        AssetLoader::Get().GetFont("default"),
        Color::Black
    ));
  }
}

void EditorScreen::LoadFile(const string &name) {
  // на всякий
  //Save();
  // восстанавливаем настройки
  curves_.erase(curves_.begin(), curves_.end());
  cur_curve_ = -1;
  cur_point_ = -1;
  cur_state_ = ADD;
  editing_ = false;
  // загрузка
  ifstream in("saves/" + name);
  if (in.is_open()) {
    string line;
    Vector2f temp;
    bool first = true;
    while (getline(in, line)) {
      curves_.emplace_back();
      uint32_t last_space = -1;
      float parsed_coord;
      for (int i = 0; i < line.size(); ++i) {
        if (line[i] == ' ') {
          parsed_coord = stof(line.substr(last_space + 1, i - last_space - 1));
          last_space = i;
          if (first) {
            temp.x = parsed_coord;
            first = false;
          } else {
            temp.y = parsed_coord;
            curves_.back().push_back(temp);
            first = true;
          }
        }
      }
    }
  }
  in.close();
}
