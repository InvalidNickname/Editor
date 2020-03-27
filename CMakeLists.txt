cmake_minimum_required(VERSION 3.14)
project(MapGeneratorDeux)
set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)

set(SOURCE_FILES src/main.cpp src/screens/screen_manager.cpp src/screens/splashscreen/splash_screen.cpp
        src/screens/editor/editor_screen.cpp src/utils/asset_loader.cpp src/utils/r.h src/screens/editor/editor_state.h)
if (WIN32)
    add_executable(MapGeneratorDeux ${SOURCE_FILES})
    #add_executable(MapGeneratorDeux WIN32 ${SOURCE_FILES})
else ()
    add_executable(MapGeneratorDeux ${SOURCE_FILES})
endif ()

# подключение SFML
message("Проверка подключения SFML")
set(SFML_INSTALL "${PROJECT_SOURCE_DIR}/libs/SFML-2.5.1")
set(SFML_DIR "${SFML_INSTALL}/lib/cmake/SFML")
set(SFML_ROOT ${SFML_INSTALL})
find_package(SFML COMPONENTS system graphics main REQUIRED)
target_link_libraries(MapGeneratorDeux sfml-system sfml-graphics sfml-main)

# библиотека интерфейса
add_subdirectory(ui)

target_include_directories(MapGeneratorDeux PUBLIC src)

target_link_libraries(MapGeneratorDeux ui)

# копирование sfml в папку с собранным приложением
add_custom_command(TARGET MapGeneratorDeux POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_INSTALL}/bin"
        $<TARGET_FILE_DIR:MapGeneratorDeux>)

# копирование ассетов
add_custom_command(TARGET MapGeneratorDeux POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        $<TARGET_FILE_DIR:MapGeneratorDeux>)

# установка
install(TARGETS MapGeneratorDeux
        RUNTIME DESTINATION build
        LIBRARY DESTINATION build)
install(DIRECTORY assets/ DESTINATION build PATTERN "last_settings.json" EXCLUDE)
install(DIRECTORY ${SFML_INSTALL}/bin/ DESTINATION build)