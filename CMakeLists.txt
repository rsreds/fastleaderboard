cmake_minimum_required(VERSION 3.28)
project(fastleaderboard)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Boost 1.83.0 REQUIRED COMPONENTS json)

# Add executable
add_executable(fastleaderboard
    main.cpp
    player.cpp
    session.cpp
    listener.cpp
)

# Link libraries
target_link_libraries(fastleaderboard
    PRIVATE
    Boost::json
)

# Include directories
target_include_directories(fastleaderboard
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)