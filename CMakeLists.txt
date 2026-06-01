cmake_minimum_required(VERSION 3.19)
project(Osci LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt_add_executable(Osci
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    plotwidget.h
    oscilloscopeform.h
    plotwidget.cpp
    oscilloscopeform.cpp
)

qt_add_translations(
    TARGETS Osci
    TS_FILES Osci_ru_RU.ts
)

target_link_libraries(Osci
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS Osci
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Osci
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
