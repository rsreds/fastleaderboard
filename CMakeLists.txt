cmake_minimum_required(VERSION 3.28)
project(fastleaderboard)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Boost 1.84.0 REQUIRED COMPONENTS json)
find_package(OpenSSL REQUIRED)
# Add executable
add_executable(fastleaderboard
src/main.cpp
src/player.cpp
src/listener.cpp
src/score.cpp
src/leaderboard.cpp
src/redis_service.cpp
src/boost_redis.cpp
)

# Include directories
target_include_directories(fastleaderboard
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(fastleaderboard PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(fastleaderboard PRIVATE Boost::json OpenSSL::SSL OpenSSL::Crypto)