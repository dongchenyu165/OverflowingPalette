cmake_minimum_required(VERSION 3.19)
project(OverflowPalette LANGUAGES CXX)

if (MSVC)
    # set(CMAKE_PREFIX_PATH "F:/QT6/6.8.1/msvc2022_64/lib/cmake")
endif()
find_package(Qt6 6.8.1 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_CXX_STANDARD 17)
if (MSVC)
    add_compile_options(/permissive-)
    add_compile_options(/Zc:__cplusplus)
    # link_directories(F:/QT6/6.8.1/msvc2022_64/lib)
endif()

qt_standard_project_setup()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
qt_add_executable(OverflowPalette
    WIN32 MACOSX_BUNDLE
    main.cpp
    overflowpalettewindow.cpp
    overflowpalettewindow.h
    overflowpalettewindow.ui
    OverflowPaletteGamePanelWidget.h OverflowPaletteGamePanelWidget.cpp OverflowPaletteGamePanelWidget.ui
    ColorSelectorWidget.h ColorSelectorWidget.cpp
    GamePanelConfig.h GamePanelConfig.cpp
    OverflowPaletteBackEnd.h
    ColorEditor.h ColorEditor.cpp ColorEditor.ui
    ColorNameInputDialog.h
    ColorEditorListWidgetItem.h
    GameConfigManager.h GameConfigManager.cpp GameConfigManager.ui
    GameEditorPanelWidget.h GameEditorPanelWidget.cpp
    GameEditorMainWidget.h GameEditorMainWidget.cpp GameEditorMainWidget.ui
    GameEditorSideToolbox.h GameEditorSideToolbox.cpp GameEditorSideToolbox.ui

)
target_link_libraries(OverflowPalette
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS OverflowPalette
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET OverflowPalette
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
