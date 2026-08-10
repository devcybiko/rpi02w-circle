cmake_minimum_required(VERSION 3.20)

project(guiapp-macos LANGUAGES C CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

set(LV_BUILD_CONF_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h"
    CACHE FILEPATH "LVGL configuration")
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(../../../lvgl lvgl)
target_link_libraries(lvgl PRIVATE PkgConfig::SDL2)

add_executable(guiapp-macos
    main.cpp
    ../shared/guiapp.cpp)
target_include_directories(guiapp-macos PRIVATE ../shared)
target_link_libraries(guiapp-macos PRIVATE lvgl PkgConfig::SDL2)
