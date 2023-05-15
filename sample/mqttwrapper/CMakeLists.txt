project(mqttwrapper VERSION 1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets DBus REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets DBus REQUIRED)

pkg_check_modules(LIBMOSQUITTO REQUIRED libmosquitto)

set(PROJECT_SOURCES
        main.cpp
        mqttWrapper.cpp mqttWrapper.h
        mainwindow.cpp mainwindow.h
        mainwindow.ui)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${LIBMOSQUITTO_LIBRARIES})
