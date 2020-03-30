cmake_minimum_required(VERSION 3.14)
project(Editor)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES src/main.cpp src/screens/screen_manager.cpp src/screens/splashscreen/splash_screen.cpp
        src/screens/editor/editor_screen.cpp src/utils/asset_loader.cpp src/utils/r.h src/screens/editor/editor_state.h)
if (WIN32)
    add_executable(Editor ${SOURCE_FILES})
    #add_executable(Editor WIN32 ${SOURCE_FILES})
else ()
    add_executable(Editor ${SOURCE_FILES})
endif ()

# подключение SFML
message("Проверка подключения SFML")
set(SFML_INSTALL "${PROJECT_SOURCE_DIR}/libs/SFML-2.5.1")
set(SFML_DIR "${SFML_INSTALL}/lib/cmake/SFML")
set(SFML_ROOT ${SFML_INSTALL})
find_package(SFML COMPONENTS system graphics main REQUIRED)
target_link_libraries(Editor sfml-system sfml-graphics sfml-main)

# библиотека интерфейса
add_subdirectory(ui)

target_include_directories(Editor PUBLIC src)

target_link_libraries(Editor ui)

# копирование sfml в папку с собранным приложением
add_custom_command(TARGET Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_INSTALL}/bin"
        $<TARGET_FILE_DIR:Editor>)

# копирование ассетов
add_custom_command(TARGET Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        $<TARGET_FILE_DIR:Editor>)

# установка
install(TARGETS Editor
        RUNTIME DESTINATION build
        LIBRARY DESTINATION build)
install(DIRECTORY assets/ DESTINATION build)
install(DIRECTORY ${SFML_INSTALL}/bin/ DESTINATION build)