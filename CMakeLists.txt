cmake_minimum_required(VERSION 3.20)
project(furtools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_VERSION 6)
find_package(Qt${QT_VERSION} REQUIRED COMPONENTS Core Gui Widgets Multimedia)

set(src_path src)

set(project_sources
  ${src_path}/format/furWavetable.cpp
  ${src_path}/format/furInstrument.cpp
  ${src_path}/format/furInstrumentFeatures.cpp

  ${src_path}/utils/interleave.cpp

  ${src_path}/common.cpp

  ${src_path}/ui/tools/utilsTool.cpp
  ${src_path}/ui/tools/configTool.cpp
  ${src_path}/ui/tools/sampleTool.cpp
  ${src_path}/ui/tool.cpp
  ${src_path}/ui/ui.cpp
  ${src_path}/ui/ui.h
  ${src_path}/main.cpp
)

if (WIN32)
  add_executable(${PROJECT_NAME} WIN32 ${project_sources})
else()
  add_executable(${PROJECT_NAME} ${project_sources})
endif()

if (MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt${QT_VERSION}::Core
  Qt${QT_VERSION}::Gui
  Qt${QT_VERSION}::Widgets
  Qt${QT_VERSION}::Multimedia
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${src_path}
  ${src_path}/ui
  ${src_path}/utils
  ${src_path}/format
)
