add_library(ui src/gui.cpp src/button.cpp src/radio_buttons.cpp src/drawable_image.cpp src/drawable_text.cpp
        src/drawable_text.h)

target_link_libraries(ui sfml-system sfml-graphics sfml-main)

target_include_directories(ui PUBLIC src)