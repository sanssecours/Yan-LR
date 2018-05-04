cmake_minimum_required (VERSION 3.0)

list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
set (CMAKE_CXX_STANDARD 11)

execute_process (COMMAND antlr4
                 RESULT_VARIABLE ANTLR_NOT_AVAILABLE OUTPUT_QUIET)
if (ANTLR_NOT_AVAILABLE)
  message (FATAL_ERROR "ANTLR 4 executable (antlr4) not found")
endif (ANTLR_NOT_AVAILABLE)

find_package (ANTLR4CPP QUIET)
if (NOT ANTLR4CPP_FOUND)
  message (FATAL_ERROR "ANTLR 4 CPP runtime (antlr4-cpp-runtime) not found")
endif (NOT ANTLR4CPP_FOUND)

set (GRAMMAR_NAME Test)
set (GRAMMAR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Grammar/${GRAMMAR_NAME}.g4)
set (GENERATED_SOURCE_FILES_NAMES Lexer Parser BaseListener Listener)
foreach (file ${GENERATED_SOURCE_FILES_NAMES})
  set (filepath ${CMAKE_CURRENT_BINARY_DIR}/${GRAMMAR_NAME}${file}.cpp)
  set_source_files_properties (${filepath} PROPERTIES GENERATED TRUE)
  list (APPEND GENERATED_SOURCE_FILES ${filepath})
endforeach (file ${GENERATED_SOURCE_FILES_NAMES})
set (SOURCE_FILES "${GENERATED_SOURCE_FILES}" main.cpp)

add_custom_command (OUTPUT ${GENERATED_SOURCE_FILES}
                    COMMAND antlr4 -Werror -Dlanguage=Cpp -o
                            ${CMAKE_CURRENT_BINARY_DIR} -package parser
                            ${GRAMMAR_FILE}
                    DEPENDS ${GRAMMAR_FILE})

include_directories ("${ANTLR4CPP_INCLUDE_DIRS}" "${CMAKE_CURRENT_BINARY_DIR}")
add_executable (badger main.cpp ${GENERATED_SOURCE_FILES})
target_link_libraries (badger ${ANTLR4CPP_LIBRARIES})
