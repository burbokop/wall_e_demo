cmake_minimum_required(VERSION 3.5)

project(km2ide LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Quick Qml REQUIRED)

set(SOURCES
    src/main.cpp
    src/treegui/kgramtreeview.cpp
    src/treegui/mainwindow.cpp
    src/treegui/appcore.cpp
    src/treegui/klibcore/kdelegate.cpp
    src/treegui/klibcore/kdelegatechooser.cpp
    src/treegui/klibcore/klibinfo.cpp
    src/treegui/asmexecutor.cpp
    src/treegui/highlighter.cpp
    src/km2/km2.cpp
    )
set(HEADERS
    src/treegui/kgramtreeview.h
    src/treegui/mainwindow.h
    src/treegui/appcore.h
    src/treegui/klibcore/kdelegate.h
    src/treegui/klibcore/kdelegatechooser.h
    src/treegui/klibcore/kmacro.h
    src/treegui/klibcore/klibinfo.h
    src/treegui/asmexecutor.h
    src/treegui/highlighter.h
    src/km2/km2.h
    )


add_executable(km2ide
    ${SOURCES}
    ${HEADERS}
)

add_library(km2ide_resources SHARED resources.qrc)

target_include_directories(km2ide PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


include(${CMAKE_CURRENT_SOURCE_DIR}/wall_e/wall_e.cmake)


target_link_libraries(km2ide
    km2ide_resources
    wall_e
    Qt5::Qml
    Qt5::Quick
)
