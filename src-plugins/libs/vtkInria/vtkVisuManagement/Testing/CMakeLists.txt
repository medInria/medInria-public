SET(KIT ${PROJECT_NAME})

SET(${PROJECT_NAME}Tests
  vtkImageFromBoundsSourceTest.cxx
  )

IF( VTKINRIA_USE_SPHERICALHARMONICS )
  CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
    ${${PROJECT_NAME}Tests}
    vtkSphericalHarmonicGlyphTest.cxx
    )
ELSE( VTKINRIA_USE_SPHERICALHARMONICS )
  CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
    ${${PROJECT_NAME}Tests}
    )
endif()


ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  ${PROJECT_NAME}
  vtkRendering 
  vtkImaging
  vtkRenderingAddOn
  )


FOREACH (test ${${PROJECT_NAME}Tests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName})
ENDFOREACH (test) 

IF( VTKINRIA_USE_SPHERICALHARMONICS )
  ADD_TEST(NAME vtkSphericalHarmonicGlyphTest COMMAND ${CXX_TEST_PATH}/${KIT}CxxTests vtkSphericalHarmonicGlyphTest "${vtkInria_DATA_DIR}/odf.nrrd" "${vtkInria_DATA_DIR}/SHTest.vtk" )
endif()
