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

foreach (copyfile ${COPYFILES})
  configure_file(	${CMAKE_CURRENT_SOURCE_DIR}/${copyfile}
		${CMAKE_CURRENT_BINARY_DIR}/${copyfile}
		COPYONLY
	  )
endforeach()

set(KIT ${PROJECT_NAME})

set(${PROJECT_NAME}Tests
  NoShader.cxx
  TestShader.cxx
  texturemap.cxx
  shadows.cxx
  fibermapper.cxx
  fibershadows.cxx
  )

create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  ${${PROJECT_NAME}Tests}
  )

add_executable(${KIT}CxxTests 
  ${Tests}
  )

target_link_libraries(${KIT}CxxTests 
  ${PROJECT_NAME}
  vtkRendering 
  vtkIO
  )


add_test(NoShader     ${CXX_TEST_PATH}/${KIT}CxxTests NoShader)
add_test(TestShader   ${CXX_TEST_PATH}/${KIT}CxxTests TestShader   "${CMAKE_CURRENT_BINARY_DIR}/testshader.prog")
#add_test(TestShader2  ${CXX_TEST_PATH}/${KIT}CxxTests TestShader   "${CMAKE_CURRENT_BINARY_DIR}/alshaderpp.prog")
add_test(texturemap   ${CXX_TEST_PATH}/${KIT}CxxTests texturemap   "${CMAKE_CURRENT_BINARY_DIR}/LogoVTK.jpg")
add_test(shadows      ${CXX_TEST_PATH}/${KIT}CxxTests shadows)
add_test(fibermapper  ${CXX_TEST_PATH}/${KIT}CxxTests fibermapper  "${CMAKE_CURRENT_BINARY_DIR}/fibers.vtk")
add_test(fibershadows ${CXX_TEST_PATH}/${KIT}CxxTests fibershadows "${CMAKE_CURRENT_BINARY_DIR}/fibers.vtk")

