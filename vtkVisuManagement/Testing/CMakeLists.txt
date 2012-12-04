SET(KIT vtkVisuManagement)

SET(vtkVisuManagementTests
  vtkImageFromBoundsSourceTest.cxx
  )

IF( vtkINRIA3D_USE_SPHERICALHARMONICS )
  CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
    ${vtkVisuManagementTests}
    vtkSphericalHarmonicGlyphTest.cxx
    )
ELSE( vtkINRIA3D_USE_SPHERICALHARMONICS )
  CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
    ${vtkVisuManagementTests}
    )
ENDIF(vtkINRIA3D_USE_SPHERICALHARMONICS )


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

IF( vtkINRIA3D_USE_SPHERICALHARMONICS )
  ADD_TEST(NAME vtkSphericalHarmonicGlyphTest COMMAND ${CXX_TEST_PATH}/${KIT}CxxTests vtkSphericalHarmonicGlyphTest "${vtkINRIA3D_DATA_DIR}/odf.nrrd" "${vtkINRIA3D_DATA_DIR}/SHTest.vtk" )
ENDIF(vtkINRIA3D_USE_SPHERICALHARMONICS )
