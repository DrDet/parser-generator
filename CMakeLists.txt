cmake_minimum_required(VERSION 3.13)
project(parser-generator)

set(CMAKE_CXX_STANDARD 14)
set(antlr4-demo-GENERATED_SRC
        ${PROJECT_SOURCE_DIR}/generated/TLexer.cpp
        ${PROJECT_SOURCE_DIR}/generated/TParser.cpp
        ${PROJECT_SOURCE_DIR}/generated/TParserBaseListener.cpp
        ${PROJECT_SOURCE_DIR}/generated/TParserBaseVisitor.cpp
        ${PROJECT_SOURCE_DIR}/generated/TParserListener.cpp
        ${PROJECT_SOURCE_DIR}/generated/TParserVisitor.cpp
        )

include_directories(${PROJECT_SOURCE_DIR}/include/antlr4-runtime)
include_directories(${PROJECT_SOURCE_DIR}/generated)
add_executable(parser-generator main.cpp ${antlr4-demo-GENERATED_SRC})
target_link_libraries(parser-generator  ${PROJECT_SOURCE_DIR}/lib/libantlr4-runtime.a)
