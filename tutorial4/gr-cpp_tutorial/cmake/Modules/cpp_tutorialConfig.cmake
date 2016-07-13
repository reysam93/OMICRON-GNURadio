INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_CPP_TUTORIAL cpp_tutorial)

FIND_PATH(
    CPP_TUTORIAL_INCLUDE_DIRS
    NAMES cpp_tutorial/api.h
    HINTS $ENV{CPP_TUTORIAL_DIR}/include
        ${PC_CPP_TUTORIAL_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    CPP_TUTORIAL_LIBRARIES
    NAMES gnuradio-cpp_tutorial
    HINTS $ENV{CPP_TUTORIAL_DIR}/lib
        ${PC_CPP_TUTORIAL_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CPP_TUTORIAL DEFAULT_MSG CPP_TUTORIAL_LIBRARIES CPP_TUTORIAL_INCLUDE_DIRS)
MARK_AS_ADVANCED(CPP_TUTORIAL_LIBRARIES CPP_TUTORIAL_INCLUDE_DIRS)

