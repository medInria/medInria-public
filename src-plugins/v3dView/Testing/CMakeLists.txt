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

set(PLUGIN_NAME v3dView)

set(v3dViewPluginTests
  v3dViewTest.cxx
  )

create_test_sourcelist(Tests ${PLUGIN_NAME}CxxTests.cxx
  ${v3dViewPluginTests}
  )

add_executable(${PLUGIN_NAME}CxxTests 
  ${Tests}
  )

target_link_libraries(${PLUGIN_NAME}CxxTests
  medGui
  medCore
  dtkCore dtkLog
  vtkImageView
  vtkHybrid
  vtkImaging
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

  set(ITK_DATA_ROOT
  )
if (ITK_SOURCE_DIR)
  set(ITK_DATA_ROOT ${ITK_SOURCE_DIR}/Examples/Data)
endif()

foreach (test ${v3dViewPluginTests})
  get_filename_component(TName ${test} NAME_WE)
  add_test(${TName} ${CXX_TEST_PATH}/${PLUGIN_NAME}CxxTests ${TName} ${CXX_TEST_PLUGIN_PATH} ${ITK_DATA_ROOT})
endforeach() 
