cmake_minimum_required(VERSION 3.0)

LIST (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
SET (CMAKE_CXX_STANDARD 11)

set (ANTLR4CPP_JAR_LOCATION "/usr/local/opt/antlr/antlr-4.7.1-complete.jar")
include (ExternalAntlr4Cpp)
include_directories ("${ANTLR4CPP_INCLUDE_DIRS}")
link_directories ("${ANTLR4CPP_LIBS}")

antlr4cpp_process_grammar (badger parser
                           ${CMAKE_CURRENT_SOURCE_DIR}/Grammar/Test.g4)
include_directories (${antlr4cpp_include_dirs_parser})
link_directories (${antlr4cpp_include_dirs_parser})

add_executable (badger main.cpp ${antlr4cpp_src_files_parser})
add_dependencies (badger antlr4cpp antlr4cpp_generation_parser)
target_link_libraries (badger antlr4-runtime)
