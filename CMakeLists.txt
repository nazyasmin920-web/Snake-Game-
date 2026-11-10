cmake_minimum_required(VERSION 3.16)
project(SnakeGame VERSION 1.0.0 LANGUAGES CXX)

# Require C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option to force downloading SFML via FetchContent
option(USE_FETCHCONTENT_SFML "Fetch SFML using CMake FetchContent if not found locally" ON)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Snake.cpp
    src/Food.cpp
    src/UI.cpp
    src/Audio.cpp
    src/Obstacle.cpp
)

set(HEADERS
    include/Game.hpp
    include/Snake.hpp
    include/Food.hpp
    include/UI.hpp
    include/Audio.hpp
    include/Obstacle.hpp
)

# ---------------------------------------------------------
# Find or Fetch SFML 2.6
# ---------------------------------------------------------
find_package(SFML 2.6 COMPONENTS graphics window system audio CONFIG QUIET)

if(NOT SFML_FOUND)
    find_package(SFML 2.5 COMPONENTS graphics window system audio QUIET)
endif()

if(NOT SFML_FOUND AND USE_FETCHCONTENT_SFML)
    message(STATUS "Local SFML installation not found. Fetching SFML 2.6.2 via FetchContent...")
    include(FetchContent)
    
    # Configure SFML options before fetching
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
    set(SFML_INSTALL_PKGCONFIG_FILES OFF CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.2
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SFML)
elseif(NOT SFML_FOUND)
    message(FATAL_ERROR "SFML was not found. Please install SFML 2.6 or enable USE_FETCHCONTENT_SFML.")
endif()

# ---------------------------------------------------------
# Executable target
# ---------------------------------------------------------
add_executable(SnakeGame ${SOURCES} ${HEADERS})

target_include_directories(SnakeGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SnakeGame PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# ---------------------------------------------------------
# Copy assets to the output directory post-build
# ---------------------------------------------------------
add_custom_command(TARGET SnakeGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:SnakeGame>/assets
    COMMENT "Copying game assets to executable folder..."
)

# On Windows, copy OpenAL and SFML DLLs if dynamic linking is used
if(WIN32)
    # Visual Studio runtime directory support
    add_custom_command(TARGET SnakeGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:SnakeGame>
            $<TARGET_FILE_DIR:SnakeGame>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLL dependencies..."
    )
endif()

