set(COPYFILES
  testshader.frag
  testshader.vert
  testshader.prog
  average.shad
  DirectionalLight.vert
  standard.vert
  standard.prog
  ppshader.vert
  ppshader.frag
  ppshader.prog
  alshader.vert
  alshader.prog
  alshaderpp.prog
  alshaderpp.vert
  alshaderpp.frag
  texture.prog
  texture.vert
  texture.frag
  LogoVTK.jpg
  fibers.vtk
  )

FOREACH(copyfile ${COPYFILES})
  configure_file(	${CMAKE_CURRENT_SOURCE_DIR}/${copyfile}
		${CMAKE_CURRENT_BINARY_DIR}/${copyfile}
		COPYONLY
	  )
ENDFOREACH(copyfile)

set(KIT ${PROJECT_NAME})

set(${PROJECT_NAME}Tests
  NoShader.cxx
  TestShader.cxx
  texturemap.cxx
  shadows.cxx
  fibermapper.cxx
  fibershadows.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${${PROJECT_NAME}Tests}
  )

ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  ${PROJECT_NAME}
  vtkRendering 
  vtkIO
  )


ADD_TEST(NoShader     ${CXX_TEST_PATH}/${KIT}CxxTests NoShader)
ADD_TEST(TestShader   ${CXX_TEST_PATH}/${KIT}CxxTests TestShader   "${CMAKE_CURRENT_BINARY_DIR}/testshader.prog")
#ADD_TEST(TestShader2  ${CXX_TEST_PATH}/${KIT}CxxTests TestShader   "${CMAKE_CURRENT_BINARY_DIR}/alshaderpp.prog")
ADD_TEST(texturemap   ${CXX_TEST_PATH}/${KIT}CxxTests texturemap   "${CMAKE_CURRENT_BINARY_DIR}/LogoVTK.jpg")
ADD_TEST(shadows      ${CXX_TEST_PATH}/${KIT}CxxTests shadows)
ADD_TEST(fibermapper  ${CXX_TEST_PATH}/${KIT}CxxTests fibermapper  "${CMAKE_CURRENT_BINARY_DIR}/fibers.vtk")
ADD_TEST(fibershadows ${CXX_TEST_PATH}/${KIT}CxxTests fibershadows "${CMAKE_CURRENT_BINARY_DIR}/fibers.vtk")

