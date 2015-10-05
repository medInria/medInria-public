################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

project(medVtkInriaTests-HWShading)

## #############################################################################
## Configure files
## #############################################################################

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


## #############################################################################
## List Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

foreach(test ${${PROJECT_NAME}_SOURCES})
    get_filename_component(test_filename ${test} NAME)
    set(${PROJECT_NAME}_TESTS_FILENAME 
      ${test_filename} 
      ${${PROJECT_NAME}_TESTS_FILENAME}
      )
    get_filename_component(test_name ${test} NAME_WE)
    set(${PROJECT_NAME}_TESTS_NAME 
      ${test_name} 
      ${${PROJECT_NAME}_TESTS_NAME}
      )
endforeach()

create_test_sourcelist(${PROJECT_NAME}_TESTS ${PROJECT_NAME}.cxx
  ${${PROJECT_NAME}_TESTS_FILENAME}
  )


## #############################################################################
## Add Exe
## #############################################################################

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_CFILES}
  ${${PROJECT_NAME}_TESTS}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  vtkRendering
  vtkIO
  medVtkInria
  )


## #############################################################################
## Add tests
## #############################################################################

add_test(NoShader     
  ${CXX_TEST_PATH}/${KIT}CxxTests 
  NoShader
  )
add_test(TestShader   
  ${CXX_TEST_PATH}/${KIT}CxxTests 
  TestShader   
  "${CMAKE_CURRENT_BINARY_DIR}/testshader.prog"
  )
#add_test(TestShader2  
  #${CXX_TEST_PATH}/${KIT}CxxTests 
  #TestShader   
  #"${CMAKE_CURRENT_BINARY_DIR}/alshaderpp.prog"
  #)
add_test(texturemap   
  ${CXX_TEST_PATH}/${KIT}CxxTests 
  texturemap   
  "${CMAKE_CURRENT_BINARY_DIR}/LogoVTK.jpg"
  )
add_test(shadows      
  ${CXX_TEST_PATH}/${KIT}CxxTests 
  shadows
  )
add_test(fibermapper  
  ${CXX_TEST_PATH}/${KIT}CxxTests 
  fibermapper  
  "${CMAKE_CURRENT_BINARY_DIR}/fibers.vtk"
  )
add_test(fibershadows 
  ${CXX_TEST_PATH}/${KIT}CxxTests 
  fibershadows 
  "${CMAKE_CURRENT_BINARY_DIR}/fibers.vtk"
  )

