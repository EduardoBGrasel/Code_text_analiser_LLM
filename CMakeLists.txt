cmake_minimum_required(VERSION 3.14) 

# project name
project(code_analise VERSION 1.0 LANGUAGES CXX)

# 2. Configurações de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------
# Download library
# ---------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3 
)

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5 
)

#
set(BUILD_CPR_TESTS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(json cpr)

# ---------------------------------------------------------
# configure files
# ---------------------------------------------------------
# search for all .cpp in the folder
file(GLOB_RECURSE SOURCES "src/*.cpp")

# creates the exec
add_executable(${PROJECT_NAME} ${SOURCES})

# include headers
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Links the downloaded library
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json cpr::cpr)