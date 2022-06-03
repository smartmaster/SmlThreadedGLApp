cmake_minimum_required(VERSION 3.23)

set(SML_PROJECT SmlThreadedGLApp)
project(${SML_PROJECT} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGL)

set(PROJECT_SOURCES
        main.cpp
        smlthreadedopenglmainwindow.cpp
        smlthreadedopenglmainwindow.h
        smlthreadedopenglmainwindow.ui
        ./Sml3DMath/SmlAxisCoord.h
        ./Sml3DMath/SmlAxisCoord.test.h
        ./SmlOpenGLWinBase/SmlSurfaceFormat.h
        ./SmlOpenGLWinBase/SmlThreadGLWindow.h
        ./SmlOpenGLWinBase/SmlWaitObject.h
        ./SmlOpenGLWinBase/SmlSurfaceFormat.cpp
        ./SmlOpenGLWinBase/SmlThreadGLWindow.cpp
        ./SmlOpenGLWinBase/SmlWaitObject.cpp
        ./SmlOpenGLWinImpl/SmlGLWindowTriangle.h
        ./SmlOpenGLWinImpl/SmlGLWindowTriangle.cpp
        ./resources/${SML_PROJECT}.qrc

        forms/openglform.h forms/openglform.cpp forms/openglform.ui
        forms/testmiscform.h forms/testmiscform.cpp forms/testmiscform.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${SML_PROJECT}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${SML_PROJECT} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${SML_PROJECT} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${SML_PROJECT}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(${SML_PROJECT} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
)

target_include_directories(${SML_PROJECT} PRIVATE
    3rdparty
    Sml3DMath
    SmlOpenGLWinBase
    SmlOpenGLWinImpl
)

set_target_properties(${SML_PROJECT} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${SML_PROJECT})
endif()
