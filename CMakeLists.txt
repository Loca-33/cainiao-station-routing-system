cmake_minimum_required(VERSION 3.19)
project(CNYZQt LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(CNYZQt
    WIN32 MACOSX_BUNDLE
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    Algorithm.h Algorithm.cpp
    FileIO.h FileIO.cpp
    Map.h Map.cpp
    Package.h
    Utils.h
    Vehicle.h
    Mapwidget.h Mapwidget.cpp
    ResultDialog.h ResultDialog.cpp
    ResultDialog.ui
)

qt_add_translations(
    TARGETS CNYZQt
    TS_FILES CNYZQt_zh_CN.ts
)

target_link_libraries(CNYZQt
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS CNYZQt
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CNYZQt
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
