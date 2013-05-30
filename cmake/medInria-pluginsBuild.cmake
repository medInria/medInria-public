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

set(${PROJECT_NAME}-plugins_INCLUDE_DIR 
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

set(${PROJECT_NAME}-plugins_INSTALL_INCLUDE_DIR 
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
