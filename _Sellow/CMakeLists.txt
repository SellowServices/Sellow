cmake_minimum_required(VERSION 3.16)

project(Sellow VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        hub.cpp
        hub.h
        hub.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Sellow
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        injectortool.h injectortool.cpp
        Resources.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Sellow APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Sellow SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Sellow
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Sellow PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID
      _OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Sellow)
endif()

set_target_properties(Sellow PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}

    ${BUNDLE_ID_OPTION}
)

include(GNUInstallDirs)
install(TARGETS Sellow
    BUNDLE DESTINATION ${CMAKE_INSTALL_LIBDIR}/Sellow/Bundles
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/Sellow/Library
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/Sellow/Runtime
    RESOURCE DESTINATION ${CMAKE_INSTALL_DATADIR}/Sellow/Resources
)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Sellow)
endif()
