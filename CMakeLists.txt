cmake_minimum_required(VERSION 3.28)
project(fastleaderboard)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Boost 1.83.0 REQUIRED COMPONENTS json)

# Add executable in the src directory
add_executable(fastleaderboard
    src/main.cpp
    src/listener.cpp
    src/session.cpp
    src/leaderboard.cpp
    src/player.cpp
    src/score.cpp
)

target_include_directories(fastleaderboard
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(fastleaderboard
    PRIVATE
    Boost::json
)
