# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Invoke macro to generate various sources
########################################################################

include (CheckCCompilerFlag)
CHECK_C_COMPILER_FLAG ("-msse2" SSE2_SUPPORTED)

if(SSE2_SUPPORTED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    add_definitions(-DFREQUENCYADAPTIVEOFDM_MSSE2)
endif(SSE2_SUPPORTED)


########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND frequencyAdaptiveOFDM_sources
    mapper_impl.cc
    signal_field_impl.cc
    decode_mac_impl.cc
    utils.cc
    chunks_to_symbols_impl.cc
    constellations_impl.cc
    equalizer/base.cc
    equalizer/comb.cc
    equalizer/ls.cc
    equalizer/lms.cc
    equalizer/sta.cc
    frame_equalizer_impl.cc
    viterbi_decoder/base.cc
    mac_and_parse_impl.cc
)

# use SSE2 optimized viterbi implementation if SSE2 is enabled
if(SSE2_SUPPORTED)
    list(APPEND frequencyAdaptiveOFDM_sources
        viterbi_decoder/viterbi_decoder_x86.cc
    )
else()
    list(APPEND frequencyAdaptiveOFDM_sources
        viterbi_decoder/viterbi_decoder_generic.cc
    )
endif(SSE2_SUPPORTED)

set(frequencyAdaptiveOFDM_sources "${frequencyAdaptiveOFDM_sources}" PARENT_SCOPE)
if(NOT frequencyAdaptiveOFDM_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT frequencyAdaptiveOFDM_sources)

add_library(gnuradio-frequencyAdaptiveOFDM SHARED ${frequencyAdaptiveOFDM_sources})
target_link_libraries(gnuradio-frequencyAdaptiveOFDM ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-frequencyAdaptiveOFDM PROPERTIES DEFINE_SYMBOL "gnuradio_frequencyAdaptiveOFDM_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-frequencyAdaptiveOFDM PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-frequencyAdaptiveOFDM RUNTIME_COMPONENT "frequencyAdaptiveOFDM_runtime" DEVEL_COMPONENT "frequencyAdaptiveOFDM_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_frequencyAdaptiveOFDM_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_frequencyAdaptiveOFDM.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_frequencyAdaptiveOFDM.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_signal_field.cc
)

add_executable(test-frequencyAdaptiveOFDM ${test_frequencyAdaptiveOFDM_sources})

target_link_libraries(
  test-frequencyAdaptiveOFDM
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-frequencyAdaptiveOFDM
)

GR_ADD_TEST(test_frequencyAdaptiveOFDM test-frequencyAdaptiveOFDM)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

