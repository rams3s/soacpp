cmake_minimum_required (VERSION 3.6)

project (soacpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Weverything -Werror -Wno-c++98-compat-pedantic -Wno-c++98-compat -Wno-missing-prototypes -Wno-unused-macros ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_program(CLANG_TIDY_EXE clang-tidy HINTS /usr/local/opt/llvm/bin/)
if (NOT ${CLANG_TIDY_EXE} STREQUAL "CLANG_TIDY_EXE-NOTFOUND")
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

include_directories(include)

add_executable(examples examples/main.cpp)

if(CLANG_TIDY_EXE)
  set_target_properties(
    examples PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
    )
endif()

# tests

set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/catch2)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

set(TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests Catch)

enable_testing()
add_test(NAME soacpp_tests COMMAND tests)
