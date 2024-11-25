cmake_minimum_required(VERSION 3.27)

project(TRA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(LIBTINS_ENABLE_CXX11 1)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

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

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1 # or the version you want to use
)
FetchContent_MakeAvailable(spdlog)
set (SPDLOG_LIBRARIES "${CMAKE_SOURCE_DIR}/_deps/spdlog-src/include")

include_directories(${libtins_SOURCE_DIR}/include)
include_directories(${date_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(TRA src/main.cpp src/db.cpp)
target_include_directories(TRA PRIVATE ${INCLUDE_DIR} ${SPDLOG_LIBRARIES})
target_link_libraries(TRA PRIVATE tins date)
set_target_properties(TRA PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(TRA PROPERTIES OUTPUT_NAME "${CMAKE_SOURCE_DIR}/build/TRA")
