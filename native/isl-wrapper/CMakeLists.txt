cmake_minimum_required(VERSION 3.16)

project(isl_wrapper LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(WIN32)
  if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "isl_wrapper supports only Windows x86_64 builds")
  endif()
elseif(APPLE)
  if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
    message(FATAL_ERROR "isl_wrapper supports only macOS aarch64 builds")
  endif()
elseif(UNIX)
  if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)$")
    message(FATAL_ERROR "isl_wrapper supports only Linux x86_64 builds")
  endif()
else()
  message(FATAL_ERROR "isl_wrapper supports only Windows, Linux, and macOS")
endif()

add_library(isl_wrapper SHARED
  src/isl_stub.c
)

target_include_directories(isl_wrapper
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
