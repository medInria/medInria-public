### medBuild.cmake ---
##
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Tue Apr  6 21:41:29 2010 (+0200)
## Version: $Id$
## Last-Updated: Tue May  3 16:50:17 2011 (+0200)
##           By: Julien Wintz
##     Update #: 2
######################################################################
##
### Commentary:
##
######################################################################
##
### Change log:
##
######################################################################

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
    ${PROJECT_SOURCE_DIR}/src-plugins/libs
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/ITKDCMTKIO
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/dtkVtkLogForwarder
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/dtItkLogForwarder
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/itkDataImageBase
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/itkProcessRegistration    
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/medPluginsPch                    
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/vtkInria
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/vtkInria/vtkDataManagement
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/vtkInria/vtkItk
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/vtkInria/vtkVisuManagement        
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/vtkInria/vtkWidgetsAddOn
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/vtkInria/vtkImageView    
    ${PROJECT_SOURCE_DIR}/src-plugins/libs/vtkInria/itkAddOn/IO/
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
    ${CMAKE_INSTALL_PREFIX}/include/libs
    ${CMAKE_INSTALL_PREFIX}/include/libs/ITKDCMTKIO
    ${CMAKE_INSTALL_PREFIX}/include/libs/dtkVtkLogForwarder
    ${CMAKE_INSTALL_PREFIX}/include/libs/dtItkLogForwarder
    ${CMAKE_INSTALL_PREFIX}/include/libs/itkDataImageBase
    ${CMAKE_INSTALL_PREFIX}/include/libs/itkProcessRegistration    
    ${CMAKE_INSTALL_PREFIX}/include/libs/medPluginsPch                    
    ${CMAKE_INSTALL_PREFIX}/include/libs/vtkInria
    ${CMAKE_INSTALL_PREFIX}/include/libs/vtkInria/vtkDataManagement
    ${CMAKE_INSTALL_PREFIX}/include/libs/vtkInria/vtkItk
    ${CMAKE_INSTALL_PREFIX}/include/libs/vtkInria/vtkVisuManagement        
    ${CMAKE_INSTALL_PREFIX}/include/libs/vtkInria/vtkWidgetsAddOn
    ${CMAKE_INSTALL_PREFIX}/include/libs/vtkInria/vtkImageView    
    ${CMAKE_INSTALL_PREFIX}/include/libs/vtkInria/itkAddOn/IO/
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
