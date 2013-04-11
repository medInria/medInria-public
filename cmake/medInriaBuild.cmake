if (WIN32)
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

## ###################################################################
## INCLUDE RULES
## ###################################################################
set(${PROJECT_NAME}_INCLUDE_DIR 
    ${PROJECT_SOURCE_DIR}/src/medCore
    ${PROJECT_SOURCE_DIR}/src/medCore/annotationData
    ${PROJECT_SOURCE_DIR}/src/medGui
    ${PROJECT_SOURCE_DIR}/src/medPacs
    ${PROJECT_SOURCE_DIR}/src/medSql
    ${PROJECT_SOURCE_DIR}/src/medTest
    ${PROJECT_SOURCE_DIR}/src/medGui/commonWidgets
    ${PROJECT_SOURCE_DIR}/src/medGui/factories
    ${PROJECT_SOURCE_DIR}/src/medGui/LUT
    ${PROJECT_SOURCE_DIR}/src/medGui/settingsWidgets
    ${PROJECT_SOURCE_DIR}/src/medGui/toolboxes
    ${PROJECT_SOURCE_DIR}/src/medGui/viewContainers
    ${PROJECT_SOURCE_DIR}/src/medGui/database
)

include_directories(${${PROJECT_NAME}_INCLUDE_DIR})

set(${PROJECT_NAME}_INSTALL_INCLUDE_DIR 
    ${CMAKE_INSTALL_PREFIX}/include/medCore
    ${CMAKE_INSTALL_PREFIX}/src/medCore/annotationData
    ${CMAKE_INSTALL_PREFIX}/include/medGui
    ${CMAKE_INSTALL_PREFIX}/include/medPacs
    ${CMAKE_INSTALL_PREFIX}/include/medSql
    ${CMAKE_INSTALL_PREFIX}/include/medTest
    ${CMAKE_INSTALL_PREFIX}/include/medGui/commonWidgets
    ${CMAKE_INSTALL_PREFIX}/include/medGui/factories
    ${CMAKE_INSTALL_PREFIX}/include/medGui/LUT
    ${CMAKE_INSTALL_PREFIX}/include/medGui/settingsWidgets
    ${CMAKE_INSTALL_PREFIX}/include/medGui/toolboxes
    ${CMAKE_INSTALL_PREFIX}/include/medGui/viewContainers
    ${CMAKE_INSTALL_PREFIX}/include/medGui/database
)


## ###################################################################
## INSTALL RULES
## ###################################################################
if( EXISTS ${${PROJECT_NAME}_BINARY_DIR}/install/${PROJECT_NAME}Config.cmake
AND EXISTS ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake
AND EXISTS ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake)

install(FILES
  ${${PROJECT_NAME}_BINARY_DIR}/install/${PROJECT_NAME}Config.cmake
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake
  DESTINATION
  cmake)

endif( EXISTS ${${PROJECT_NAME}_BINARY_DIR}/install/${PROJECT_NAME}Config.cmake
   AND EXISTS ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake
   AND EXISTS ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake)
