cmake_minimum_required(VERSION 3.13)
project(parser-generator)

set(CMAKE_CXX_STANDARD 14)

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/generated parser-generator-GENERATED_SRC)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/src parser-generator-SRC)

include_directories(${PROJECT_SOURCE_DIR}/include/antlr4-runtime)
include_directories(${PROJECT_SOURCE_DIR}/generated)
include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(parser-generator ${parser-generator-SRC} ${parser-generator-GENERATED_SRC})
target_link_libraries(parser-generator  ${PROJECT_SOURCE_DIR}/lib/libantlr4-runtime.a)
