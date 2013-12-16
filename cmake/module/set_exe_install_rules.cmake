################################################################################
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
################################################################################

macro(set_exe_install_rules
  project_name 
  )

install(TARGETS ${project_name}
  RUNTIME DESTINATION bin
  BUNDLE  DESTINATION bin
  )


if (APPLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME 
    ${PROJECT_NAME}
    )
  set(MACOSX_BUNDLE_ICON_FILE 
    medInria.icns
    )
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING 
    ${${PROJECT_NAME}_VERSION}
    )
  set(MACOSX_BUNDLE_BUNDLE_VERSION 
    ${${PROJECT_NAME}_VERSION}
    )
  set(MACOSX_BUNDLE_LONG_VERSION_STRING 
    "Version ${${PROJECT_NAME}_VERSION}"
    )
  set(${PROJECT_NAME}_RESOURCE_DIR 
    ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.app/Contents/Resources
    )
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${${PROJECT_NAME}_RESOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE__CURRENT_SOURCE_DIR}/ressources/${project_name}.icns ${${PROJECT_NAME}_RESOURCE_DIR}
    )
  
  install(CODE "
  execute_process(COMMAND 
    \${QT_BINARY_DIR}/macdeployqt \${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app
    )
	execute_process(COMMAND 
      \${dtk_DIR}/bin/dtkDeploy \${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app -inject-dir=\${CMAKE_CURRENT_BINARY_DIR}/plugins
	  )
  ")
endif()
  
endmacro()

