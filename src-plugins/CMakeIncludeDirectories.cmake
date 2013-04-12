SET(${PROJECT_NAME}_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/libs
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/ITKDCMTKIO
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/dtkVtkLogForwarder
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/dtItkLogForwarder
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/itkDataImageBase
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/itkProcessRegistration    
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/medPluginsPch                    
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/vtkInria
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/vtkInria/vtkDataManagement
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/vtkInria/vtkItk
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/vtkInria/vtkVisuManagement        
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/vtkInria/vtkWidgetsAddOn
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/vtkInria/vtkImageView    
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/vtkInria/itkAddOn/IO/
)

include_directories(
        ${${PROJECT_NAME}_INCLUDE_DIR}
)