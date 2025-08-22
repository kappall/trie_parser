cmake_minimum_required(VERSION 3.15)
project(trie_parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define executable
add_executable(trie_app
    src/main.cpp
)

# Where to find headers
target_include_directories(trie_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Sanitizers & warnings (only in Debug mode)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(trie_app PRIVATE
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion -O0 -g
        -fsanitize=address,undefined
    )
    target_link_options(trie_app PRIVATE
        -fsanitize=address,undefined
    )
endif()

find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()