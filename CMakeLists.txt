cmake_minimum_required (VERSION 3.0)

list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=integer")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

execute_process (COMMAND antlr RESULT_VARIABLE ANTLR_NOT_AVAILABLE OUTPUT_QUIET)
if (ANTLR_NOT_AVAILABLE)
  message (FATAL_ERROR "ANTLR 4 executable (antlr) not found")
endif (ANTLR_NOT_AVAILABLE)

find_package (ANTLR4CPP QUIET)
if (NOT ANTLR4CPP_FOUND)
  message (FATAL_ERROR "ANTLR 4 CPP runtime (antlr4-cpp-runtime) not found")
endif (NOT ANTLR4CPP_FOUND)

find_package (Elektra REQUIRED)
if (ELEKTRA_FOUND)
  message (STATUS "Elektra ${ELEKTRA_VERSION} found")
  include_directories (${ELEKTRA_INCLUDE_DIR})
else (ELEKTRA_FOUND)
  message (FATAL_ERROR "Elektra not found")
endif (ELEKTRA_FOUND)

find_path (spdlog_INCLUDE_DIR
           NAMES spdlog/spdlog.h
           DOC "spdlog library header files")
if (spdlog_INCLUDE_DIR STREQUAL spdlog_INCLUDE_DIR-NOTFOUND)
  message (FATAL_ERROR "spdlog not found")
endif (spdlog_INCLUDE_DIR STREQUAL spdlog_INCLUDE_DIR-NOTFOUND)

set (GRAMMAR_NAME YAML)
set (GRAMMAR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Grammar/${GRAMMAR_NAME}.g4)
set (GENERATED_SOURCE_FILES_NAMES BaseListener Listener)
foreach (file ${GENERATED_SOURCE_FILES_NAMES} "")
  set (filepath ${CMAKE_CURRENT_BINARY_DIR}/${GRAMMAR_NAME}${file}.cpp)
  set_source_files_properties (${filepath} PROPERTIES GENERATED TRUE)
  list (APPEND GENERATED_SOURCE_FILES ${filepath})
endforeach (file ${GENERATED_SOURCE_FILES_NAMES})

set (SOURCE_FILES
     "${GENERATED_SOURCE_FILES}"
     Source/main.cpp
     Source/ErrorListener.hpp
     Source/ErrorListener.cpp
     Source/Listener.hpp
     Source/Listener.cpp
     Source/YAMLLexer.hpp
     Source/YAMLLexer.cpp)

add_custom_command (OUTPUT ${GENERATED_SOURCE_FILES}
                    COMMAND antlr -Werror -Dlanguage=Cpp -o
                            ${CMAKE_CURRENT_BINARY_DIR} -package antlr
                            ${GRAMMAR_FILE}
                    DEPENDS ${GRAMMAR_FILE}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Grammar)

include_directories ("${ANTLR4CPP_INCLUDE_DIRS}" "${CMAKE_CURRENT_BINARY_DIR}"
                     "${spdlog_INCLUDE_DIR}")
add_executable (badger ${SOURCE_FILES})
target_link_libraries (badger ${ANTLR4CPP_LIBRARIES} elektra)
