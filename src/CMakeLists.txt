add_definitions("-D_DEFAULT_SOURCE") # for htole32()
add_definitions("-D_POSIX_C_SOURCE=200112L") # for nanosleep() and lroundf()

# Express a preference for C99 format strings when using MinGW, see:
# https://sourceforge.net/p/mingw-w64/wiki2/gnu%20printf/
add_definitions("-D__USE_MINGW_ANSI_STDIO=1")

# Find packages needed to build library
find_package(libusb-1.0 REQUIRED)
include_directories(${LIBUSB_1_INCLUDE_DIRS})

set(SRC am7xxx.c serialize.c tools.c)

# Build the library
add_library(am7xxx SHARED ${SRC})
set_target_properties(am7xxx PROPERTIES
  VERSION ${PROJECT_VER}
  SOVERSION ${PROJECT_APIVER})
install(TARGETS am7xxx
  DESTINATION "${CMAKE_INSTALL_LIBDIR}")

add_library(am7xxx-static STATIC ${SRC})
set_target_properties(am7xxx-static PROPERTIES OUTPUT_NAME am7xxx)
if(UNIX AND NOT APPLE)
  set_target_properties(am7xxx-static PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
install(TARGETS am7xxx-static
  DESTINATION "${CMAKE_INSTALL_LIBDIR}")

if(NOT WIN32)
  find_library(MATH_LIB m)
else()
  # not needed on windows
  set(MATH_LIB "")
endif()

target_link_libraries(am7xxx ${MATH_LIB} ${LIBUSB_1_LIBRARIES})
target_link_libraries(am7xxx-static ${MATH_LIB} ${LIBUSB_1_LIBRARIES})

# Install the header files
install(FILES "am7xxx.h"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

if(UNIX AND NOT APPLE)
  # Produce a pkg-config file for linking against the shared lib
  configure_file("libam7xxx.pc.in" "libam7xxx.pc" @ONLY)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libam7xxx.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()
