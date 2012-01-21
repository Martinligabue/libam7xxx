add_definitions("-D_BSD_SOURCE") # for htole32()
add_definitions("-D_POSIX_C_SOURCE=2") # for getopt()

# Find packages needed to build library
find_package(libusb-1.0 REQUIRED)
include_directories(${LIBUSB_1_INCLUDE_DIRS})

set(SRC am7xxx.c)

# Build the library
add_library(am7xxx SHARED ${SRC})
set_target_properties(am7xxx PROPERTIES
  VERSION ${PROJECT_VER}
  SOVERSION ${PROJECT_APIVER})
install(TARGETS am7xxx
  DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

add_library(am7xxx-static STATIC ${SRC})
set_target_properties(am7xxx-static PROPERTIES OUTPUT_NAME am7xxx)
if(UNIX AND NOT APPLE)
  set_target_properties(am7xxx-static PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
install(TARGETS am7xxx-static
   DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

target_link_libraries(am7xxx ${LIBUSB_1_LIBRARIES})
target_link_libraries(am7xxx-static ${LIBUSB_1_LIBRARIES})

# Install the header files
install(FILES "am7xxx.h"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/include")

if(UNIX AND NOT APPLE)
  # Produce a pkg-config file for linking against the shared lib
  configure_file ("libam7xxx.pc.in" "libam7xxx.pc" @ONLY)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libam7xxx.pc"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
endif()

# Build the test app
add_executable(picoproj picoproj.c)
target_link_libraries(picoproj am7xxx)
install(TARGETS picoproj
  DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")