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

set(KIT ${PROJECT_NAME})

set(${PROJECT_NAME}Tests
  vtkImageFromBoundsSourceTest.cxx
  )

if ( VTKINRIA_USE_SPHERICALHARMONICS )
  create_test_sourcelist(Tests ${KIT}CxxTests.cxx
    ${${PROJECT_NAME}Tests}
    vtkSphericalHarmonicGlyphTest.cxx
    )
else()
  create_test_sourcelist(Tests ${KIT}CxxTests.cxx
    ${${PROJECT_NAME}Tests}
    )
endif()


add_executable(${KIT}CxxTests 
  ${Tests}
  )

target_link_libraries(${KIT}CxxTests 
  ${PROJECT_NAME}
  vtkRendering 
  vtkImaging
  vtkRenderingAddOn
  )


foreach (test ${${PROJECT_NAME}Tests})
  get_filename_component(TName ${test} NAME_WE)
  add_test(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName})
endforeach() 

if ( VTKINRIA_USE_SPHERICALHARMONICS )
  add_test(NAME vtkSphericalHarmonicGlyphTest COMMAND ${CXX_TEST_PATH}/${KIT}CxxTests vtkSphericalHarmonicGlyphTest "${vtkInria_DATA_DIR}/odf.nrrd" "${vtkInria_DATA_DIR}/SHTest.vtk" )
endif()
