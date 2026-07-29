cmake_minimum_required(VERSION 3.20)

project(I_HAVE_A_DREAM VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

option(ENABLE_TESTS "Enable tests" OFF)

# ============================================================
# raylib dependency
# ============================================================
# Priority:
# 1. Use local third_party/raylib if it exists
# 2. Otherwise download raylib using FetchContent
# ============================================================

set(LOCAL_RAYLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/raylib")

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

if(EXISTS "${LOCAL_RAYLIB_DIR}/CMakeLists.txt")
    message(STATUS "Using local raylib from third_party/raylib")

    add_subdirectory(
        "${LOCAL_RAYLIB_DIR}"
        "${CMAKE_BINARY_DIR}/third_party/raylib"
        EXCLUDE_FROM_ALL
    )
else()
    message(STATUS "Local raylib not found. Fetching raylib from GitHub.")

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(raylib)
endif()

# ============================================================
# Game domain library
# ============================================================

set(GAME_DOMAIN_SOURCES
    src/character.cpp
    src/combat_system.cpp
    src/player.cpp
    src/player_controller.cpp
    src/player_shadow.cpp
    src/player_sprite_renderer.cpp
    src/platform_system.cpp
    src/shadow_manager.cpp
    src/enemy.cpp
    src/melee_enemy.cpp
    src/ranged_enemy.cpp
    src/projectile_system.cpp
    src/item.cpp
)

add_library(game_domain ${GAME_DOMAIN_SOURCES})

target_link_libraries(game_domain PUBLIC raylib)

target_include_directories(game_domain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================
# Main executable
# ============================================================

add_executable(I_HAVE_A_DREAM
    src/main.cpp
    src/scene.cpp
)

# Keep the executable directly in build/ for both single- and multi-config
# generators so the VS Code launch/task paths stay consistent.
set_target_properties(I_HAVE_A_DREAM PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}"
)

target_link_libraries(I_HAVE_A_DREAM PRIVATE
    game_domain
    raylib
)

# 将资源复制到可执行文件旁边，使从 build/ 目录直接运行时也能找到素材。
add_custom_command(TARGET I_HAVE_A_DREAM POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:I_HAVE_A_DREAM>/assets"
)

# ============================================================
# Warnings
# ============================================================

function(enable_project_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

enable_project_warnings(game_domain)
enable_project_warnings(I_HAVE_A_DREAM)

# macOS frameworks needed by raylib
if(APPLE)
    target_link_libraries(I_HAVE_A_DREAM PRIVATE
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
    )
endif()

# ============================================================
# Tests
# ============================================================

if(ENABLE_TESTS)
    include(CTest)

    # ========================================================
    # GoogleTest dependency
    # ========================================================
    # Priority:
    # 1. Use local third_party/googletest if it exists
    # 2. Otherwise download GoogleTest using FetchContent
    # ========================================================

    set(LOCAL_GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest")

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    if(EXISTS "${LOCAL_GTEST_DIR}/CMakeLists.txt")
        message(STATUS "Using local GoogleTest from third_party/googletest")

        add_subdirectory(
            "${LOCAL_GTEST_DIR}"
            "${CMAKE_BINARY_DIR}/third_party/googletest"
            EXCLUDE_FROM_ALL
        )
    else()
        message(STATUS "Local GoogleTest not found. Fetching GoogleTest from GitHub.")

        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.16.0
            GIT_SHALLOW TRUE
        )

        FetchContent_MakeAvailable(googletest)
    endif()

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cpp"
    )

    if(NOT TEST_SOURCES)
        message(FATAL_ERROR
            "No test sources found. Add at least one file matching tests/*_test.cpp."
        )
    endif()

    add_executable(game_tests
        ${TEST_SOURCES}
        src/scene.cpp
    )

    target_link_libraries(game_tests PRIVATE
        game_domain
        GTest::gtest_main
        raylib
    )

    enable_project_warnings(game_tests)

    include(GoogleTest)
    gtest_discover_tests(game_tests)

    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    add_test(
        NAME test_file_coverage
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_test_coverage.py
            --src-dir ${CMAKE_CURRENT_SOURCE_DIR}/src
            --tests-dir ${CMAKE_CURRENT_SOURCE_DIR}/tests
            --exclude main.cpp
    )
endif()
