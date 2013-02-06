SET(KIT vtkVisuManagement)

SET(vtkVisuManagementTests
  vtkImageFromBoundsSourceTest.cxx
  )

IF( vtkInria_USE_SPHERICALHARMONICS )
  CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
    ${vtkVisuManagementTests}
    vtkSphericalHarmonicGlyphTest.cxx
    )
ELSE( vtkInria_USE_SPHERICALHARMONICS )
  CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
    ${vtkVisuManagementTests}
    )
ENDIF(vtkInria_USE_SPHERICALHARMONICS )


ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  vtkVisuManagement
  vtkRendering 
  vtkImaging
  vtkRenderingAddOn
  )


FOREACH (test ${vtkVisuManagementTests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName})
ENDFOREACH (test) 

IF( vtkInria_USE_SPHERICALHARMONICS )
  ADD_TEST(NAME vtkSphericalHarmonicGlyphTest COMMAND ${CXX_TEST_PATH}/${KIT}CxxTests vtkSphericalHarmonicGlyphTest "${vtkInria_DATA_DIR}/odf.nrrd" "${vtkInria_DATA_DIR}/SHTest.vtk" )
ENDIF(vtkInria_USE_SPHERICALHARMONICS )
