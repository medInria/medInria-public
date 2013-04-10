set(KIT ${PROJECT_NAME})

set(${PROJECT_NAME}Tests
  vtkImageFromBoundsSourceTest.cxx
  )

if ( VTKINRIA_USE_SPHERICALHARMONICS )
  CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
    ${${PROJECT_NAME}Tests}
    vtkSphericalHarmonicGlyphTest.cxx
    )
else()
  CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
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


FOREACH (test ${${PROJECT_NAME}Tests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  add_test(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName})
ENDFOREACH (test) 

if ( VTKINRIA_USE_SPHERICALHARMONICS )
  add_test(NAME vtkSphericalHarmonicGlyphTest COMMAND ${CXX_TEST_PATH}/${KIT}CxxTests vtkSphericalHarmonicGlyphTest "${vtkInria_DATA_DIR}/odf.nrrd" "${vtkInria_DATA_DIR}/SHTest.vtk" )
endif()
