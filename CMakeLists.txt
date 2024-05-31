cmake_minimum_required(VERSION 3.0)
project(libam7xxx C)

include(GNUInstallDirs)

set(PROJECT_DESCRIPTION "Communication library for Actions Micro AM7XXX based USB projectors and DPFs")

set(PROJECT_VER_MAJOR 0)
set(PROJECT_VER_MINOR 1)
set(PROJECT_VER_PATCH 7)
set(PROJECT_VER_EXTRA "")
set(PROJECT_VER "${PROJECT_VER_MAJOR}.${PROJECT_VER_MINOR}.${PROJECT_VER_PATCH}${PROJECT_VER_EXTRA}")
set(PROJECT_APIVER "${PROJECT_VER_MAJOR}.${PROJECT_VER_MINOR}")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

option(STRICT_COMPILATION_CHECKS "Enable stricter compilation checks" OFF)

include(MaintenanceTools)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(DOC_OUTPUT_PATH ${CMAKE_BINARY_DIR}/doc)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_definitions(-Wall)

  if(CMAKE_C_FLAGS STREQUAL "")
    set(CMAKE_C_FLAGS "-std=c99 -pedantic -Wall -Wextra")
  endif()

  set(COMMON_FLAGS
    "-fvisibility=hidden"
    "-fno-common"
    "-Wall"
    "-Wextra"
    "-Wformat=2"
    "-Winit-self"
    "-Winline"
    "-Wpacked"
    "-Wpointer-arith"
    "-Wlarger-than-65500"
    "-Wmissing-declarations"
    "-Wmissing-format-attribute"
    "-Wmissing-noreturn"
    "-Wmissing-prototypes"
    "-Wnested-externs"
    "-Wold-style-definition"
    "-Wredundant-decls"
    "-Wsign-compare"
    "-Wstrict-aliasing=2"
    "-Wstrict-prototypes"
    "-Wswitch-enum"
    "-Wundef"
    "-Wunreachable-code"
    "-Wwrite-strings"
  )

  string(REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")

  include(CheckCCompilerFlag)
  CHECK_C_COMPILER_FLAG(-fstack-protector-strong COMPILER_SUPPORTS_STACK_PROTECTOR_STRONG)
  if (COMPILER_SUPPORTS_STACK_PROTECTOR_STRONG)
    string(APPEND CMAKE_C_FLAGS " -fstack-protector-strong")
  else()
    string(APPEND CMAKE_C_FLAGS " -fstack-protector")
  endif()

  set(DEBUG_FLAGS
    "-ggdb"
  )

  set(RELEASE_FLAGS
    "-Wp,-D_FORTIFY_SOURCE=2"
  )

  if (STRICT_COMPILATION_CHECKS)
    set(STRICT_FLAGS
      "-Werror"
      # "-Wsign-conversion"
      "-pedantic-errors"
    )
    string(APPEND CMAKE_C_FLAGS " ${STRICT_FLAGS}")
  endif()

  if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    string(APPEND CMAKE_C_FLAGS " -Wunsafe-loop-optimizations --param=ssp-buffer-size=4")

    if (STRICT_COMPILATION_CHECKS)
      string(APPEND CMAKE_C_FLAGS " -Wunused-but-set-variable")
    endif()
  elseif (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    if (STRICT_COMPILATION_CHECKS)
      string(APPEND CMAKE_C_FLAGS " -Wshorten-64-to-32")
    endif()
  endif()

  set(CMAKE_C_FLAGS_DEBUG "-O0 -DDEBUG=1 ${CMAKE_C_FLAGS} ${DEBUG_FLAGS}")
  set(CMAKE_C_FLAGS_RELEASE "-O2 ${CMAKE_C_FLAGS} ${RELEASE_FLAGS}")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 ${CMAKE_C_FLAGS} ${RELEASE_FLAGS} ${DEBUG_FLAGS}")
endif()

# Add library project
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(doc)
