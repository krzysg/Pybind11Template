###############################################################################
# APR - Adaptive Particle Representation
###############################################################################
cmake_minimum_required(VERSION 3.9)
project(Pybind11Template DESCRIPTION "Template for pybind11 / playground")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/)

###############################################################################
# Configure compiler options
###############################################################################
# If you ever want to compile with Intel's icc (or any other compiler) provide
# compiler names/paths in cmake command like this:
# CC="icc" CXX="icc" CXXFLAGS="-O3" cmake -DAPR_TESTS=1
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic")
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS_RELEASE "-O4 -ffast-math")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Bdynamic")
    if(NOT WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl")
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
endif()

###############################################################################
# PYTHON wrappers
###############################################################################
    message(STATUS "----> Building PYTHON wrappers")
    set(PYBIND11_PYTHON_VERSION 2.7)

    add_subdirectory("external/pybind11")

    set(TEMPLATE_PYTHON_MODULE_NAME pyTemplate)
    add_definitions(-DTEMPLATE_PYTHON_MODULE_NAME=${TEMPLATE_PYTHON_MODULE_NAME})
    add_library(${TEMPLATE_PYTHON_MODULE_NAME} MODULE src/pythonBind.cpp )
    target_include_directories(${APR_PYTHON_MODULE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(${TEMPLATE_PYTHON_MODULE_NAME} PRIVATE pybind11::module)
    set_target_properties(${TEMPLATE_PYTHON_MODULE_NAME} PROPERTIES OUTPUT_NAME ${TEMPLATE_PYTHON_MODULE_NAME})
    set_target_properties(${TEMPLATE_PYTHON_MODULE_NAME} PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                                                    SUFFIX "${PYTHON_MODULE_EXTENSION}")

