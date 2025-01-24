cmake_minimum_required(VERSION 3.19)
project(OverflowPalette LANGUAGES CXX)

find_package(Qt6 6.8.1 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_CXX_STANDARD 17)
if (MSVC)
    add_compile_options(/permissive-)
    add_compile_options(/Zc:__cplusplus)
endif()

qt_standard_project_setup()

# QT source files related directories
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(UI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ui_files)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${UI_DIR})

# Scan all source and ui files.
file(GLOB_RECURSE PROJECT_SOURCES 
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h"

    "${UI_DIR}/*.ui"
)

set(INCLUDE_DIRECTORIES
    ${SOURCE_DIR}/core
    ${SOURCE_DIR}/ui/editor
    ${SOURCE_DIR}/ui/game
    ${SOURCE_DIR}/ui/widgets
    ${SOURCE_DIR}/
    ${UI_DIR}
)

qt_add_executable(OverflowPalette
    WIN32 MACOSX_BUNDLE
    ${PROJECT_SOURCES}
)
target_include_directories(OverflowPalette PRIVATE ${INCLUDE_DIRECTORIES})

target_link_libraries(OverflowPalette
    PRIVATE
        Qt::Core
        Qt::Widgets
)

# Deploy Qt dependencies
if(WIN32)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    
    # Set program directory
    set(PROGRAM_OUTPUT_DIR ${CMAKE_BINARY_DIR}/program)

    # Make sure program directory exists
    file(MAKE_DIRECTORY ${PROGRAM_OUTPUT_DIR})
    # Copy executable to program directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
            $<TARGET_FILE:${PROJECT_NAME}>
            ${PROGRAM_OUTPUT_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>
        COMMAND "${_qt_bin_dir}/windeployqt.exe"
                --no-compiler-runtime
                --no-translations
                --no-system-d3d-compiler
                \"${PROGRAM_OUTPUT_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>\"
        COMMENT "Copying executable and deploying Qt dependencies to program directory..."
    )
endif()

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
