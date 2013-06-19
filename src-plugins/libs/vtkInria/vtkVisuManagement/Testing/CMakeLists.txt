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

set(testname ${PROJECT_NAME}CxxTests)

set(${PROJECT_NAME}Tests vtkImageFromBoundsSourceTest.cxx)

set(tests ${testname}.cxx ${${PROJECT_NAME}Tests})
if (VTKINRIA_USE_SPHERICALHARMONICS)
    set(vtkSphericalHarmonicGlyphTestARGS "${vtkInria_DATA_DIR}/odf.nrrd" "${vtkInria_DATA_DIR}/SHTest.vtk")
    set(${PROJECT_NAME}Tests ${${PROJECT_NAME}Tests} vtkSphericalHarmonicGlyphTest.cxx)
endif()
create_test_sourcelist(Tests ${test})

add_executable(${testname} ${Tests})

target_link_libraries(${testname}
    ${PROJECT_NAME}
    vtkRendering 
    vtkImaging
    vtkRenderingAddOn
)

foreach (test ${${PROJECT_NAME}Tests})
    get_filename_component(TName ${test} NAME_WE)
    add_test(NAME ${TName} COMMAND ${CXX_TEST_PATH}/${testname} ${TName} ${${TName}ARGS})
endforeach() 
