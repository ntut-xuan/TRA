cmake_minimum_required(VERSION 3.27)

project(TRA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(LIBTINS_ENABLE_CXX11 1)

set (SRCS
    src/main.cpp
)

include(FetchContent)

FetchContent_Declare(
    libtins
    GIT_REPOSITORY https://github.com/mfontanini/libtins.git
    GIT_TAGS v4.5
)
FetchContent_MakeAvailable(libtins)

FetchContent_Declare(
    date
    GIT_REPOSITORY https://github.com/HowardHinnant/date.git
    GIT_TAGS v3.0.3
)
FetchContent_MakeAvailable(date)

include_directories(${libtins_SOURCE_DIR}/include)
include_directories(${date_SOURCE_DIR}/include)
include_directories(${CMAKE_BINARY_DIR}/include)

add_executable(TRA ${SRCS})
target_include_directories(TRA PRIVATE ${CMAKE_BINARY_DIR}/include)
target_link_libraries(TRA PRIVATE tins date)
set_target_properties(TRA PROPERTIES OUTPUT_NAME "${CMAKE_BINARY_DIR}/build/TRA")
