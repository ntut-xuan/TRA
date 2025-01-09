cmake_minimum_required(VERSION 3.27)

project(TRA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3 -g")
set(LIBTINS_ENABLE_CXX11 1)

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
set (SPDLOG_LIBRARIES "${CMAKE_BINARY_DIR}/_deps/spdlog-src/include")

FetchContent_Declare(
  sockpp
  GIT_REPOSITORY https://github.com/fpagliughi/sockpp.git
  GIT_TAGS v1.0.0
)
FetchContent_MakeAvailable(sockpp)
set (SOCKPP_LIBRARIES "${CMAKE_BINARY_DIR}/_deps/sockpp-src/include")

FetchContent_Declare(
  argh
  GIT_REPOSITORY https://github.com/adishavit/argh.git
  GIT_TAGS v1.3.2
)
FetchContent_MakeAvailable(argh)
set (ARGH_LIBRARIES "${CMAKE_BINARY_DIR}/_deps/argh-src/")

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)
set (JSON_LIBRARIES "${CMAKE_BINARY_DIR}/_deps/json-src/include")

include_directories(${libtins_SOURCE_DIR}/include)
include_directories(${date_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${SOCKPP_LIBRARIES})
include_directories(${ARGH_LIBRARIES})

add_executable(TRA  node/src/main.cpp  node/src/db.cpp node/src/config.cpp)
target_include_directories(TRA PRIVATE ${INCLUDE_DIR} ${SPDLOG_LIBRARIES} ${ARGH_LIBRARIES} ${JSON_LIBRARIES})
target_link_libraries(TRA PRIVATE tins date argh)
set_target_properties(TRA PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(TRA PROPERTIES OUTPUT_NAME "${CMAKE_SOURCE_DIR}/build/TRA")

add_executable(TRA-REPORTER reporter/src/main.cpp)
target_include_directories(TRA-REPORTER PRIVATE ${INCLUDE_DIR} ${SPDLOG_LIBRARIES} ${SOCKPP_LIBRARIES})
target_link_libraries(TRA-REPORTER PRIVATE tins date sockpp)
set_target_properties(TRA-REPORTER PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(TRA-REPORTER PROPERTIES OUTPUT_NAME "${CMAKE_SOURCE_DIR}/build/TRA-REPORTER")