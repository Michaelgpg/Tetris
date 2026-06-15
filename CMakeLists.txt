cmake_minimum_required(VERSION 3.20)
project(Tetris)

set(CMAKE_CXX_STANDARD 17)

# Указываем пути к SFML вручную
set(SFML_INCLUDE_DIR "/usr/include/SFML")
set(SFML_LIBRARY_DIR "/usr/lib")

include_directories(${SFML_INCLUDE_DIR})
link_directories(${SFML_LIBRARY_DIR})

add_executable(Tetris main.cpp)

# Линкуем библиотеки напрямую
target_link_libraries(Tetris
        sfml-graphics
        sfml-window
        sfml-system
)