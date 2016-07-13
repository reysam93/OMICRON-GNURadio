# Install script for directory: /home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/python

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/python_tutorial" TYPE FILE FILES
    "/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/python/__init__.py"
    "/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/python/multiply_ff.py"
    "/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/python/qpsk_demod_py_cb.py"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/python_tutorial" TYPE FILE FILES
    "/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/build/python/__init__.pyc"
    "/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/build/python/multiply_ff.pyc"
    "/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/build/python/qpsk_demod_py_cb.pyc"
    "/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/build/python/__init__.pyo"
    "/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/build/python/multiply_ff.pyo"
    "/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/build/python/qpsk_demod_py_cb.pyo"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

