cmake_minimum_required(VERSION 3.16)
project(MusicPlayer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt modules - try Qt6 first, fallback to Qt5
find_package(Qt6 COMPONENTS Core Widgets Multimedia MultimediaWidgets QUIET)

if(NOT Qt6_FOUND)
    message(STATUS "Qt6 not found, trying Qt5...")
    find_package(Qt5 5.15 COMPONENTS Core Widgets Multimedia MultimediaWidgets REQUIRED)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

message(STATUS "Using Qt${QT_VERSION_MAJOR}")

# Enable automatic handling of Qt classes
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find TagLib for audio metadata
find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED taglib)

# Source files
set(SOURCES
    main.cpp
    musicplayer.cpp
)

set(HEADERS
    musicplayer.h
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# Link Qt libraries
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        ${TAGLIB_LIBRARIES}
    )
else()
    target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Widgets
        Qt5::Multimedia
        Qt5::MultimediaWidgets
        ${TAGLIB_LIBRARIES}
    )
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${TAGLIB_INCLUDE_DIRS}
)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
