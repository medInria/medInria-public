#######################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#######################################################################

set(PLUGIN_NAME itkDCMTKDataImageReaderPlugin)

set(${PLUGIN_NAME}Tests
  itkDCMTKDataImageReaderTest.cxx
)

create_test_sourcelist(Tests ${PLUGIN_NAME}CxxTests.cxx
  ${${PLUGIN_NAME}Tests}
  )

add_executable(${PLUGIN_NAME}CxxTests 
  ${Tests}
  )

target_link_libraries(${PLUGIN_NAME}CxxTests
  dtkCore dtkLog
  medCore
  ${QT_LIBRARIES}
  )

set_target_properties(${PLUGIN_NAME}CxxTests PROPERTIES 
RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
)

set(CXX_TEST_PATH        ${EXECUTABLE_OUTPUT_PATH})
set(CXX_TEST_PLUGIN_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
if (MSVC)
  set(CXX_TEST_PLUGIN_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release)
endif()

find_path(DICOM_DATA_ROOT "dicom.lst")

get_filename_component(TName ${${PLUGIN_NAME}Tests} NAME_WE)

if (DICOM_DATA_ROOT)
  file(STRINGS "${DICOM_DATA_ROOT}/dicom.lst" DICOM_LIST)
  foreach(DICOM_DIR ${DICOM_LIST})
    add_test(${PLUGIN_NAME}_${DICOM_DIR} ${CXX_TEST_PATH}/${PLUGIN_NAME}CxxTests ${TName} ${CXX_TEST_PLUGIN_PATH} "${DICOM_DATA_ROOT}/${DICOM_DIR}")
  endforeach()
endif()
