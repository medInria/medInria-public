### CMakeLists.txt --- 
## 
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(medData)

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
    medAbstractData.h
    medAbstractDataImage.h
    medAbstractDataImage2D.h
    medAbstractDataImage3D.h
    medAbstractDataImage4D.h
    medAbstractDataMesh.h
    medAttachedData.h
)

set(${PROJECT_NAME}_HEADERS
    ${${PROJECT_NAME}_HEADERS_MOC}
    medAbstractDataCollection.h
    medAbstractDataTypedImage.h
    medAbstractDataTypedImage2D.h
    medAbstractDataTypedImage3D.h
    medAbstractDataTypedImage4D.h
    medDataExport.h
)


set(${PROJECT_NAME}_SOURCES
    medAbstractData.cpp
    medAbstractDataCollection.cpp
    medAbstractDataImage.cpp
    medAbstractDataImage2D.cpp
    medAbstractDataImage3D.cpp
    medAbstractDataImage4D.cpp
    medAbstractDataMesh.cpp
    medAttachedData.cpp
)

## #################################################################
## Build rules
## #################################################################

if(NOT MSVC)
    add_definitions(-Wno-write-strings)
    add_definitions(-Wformat=0)
endif(NOT MSVC)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_SHARED)
add_definitions(-DQT_NO_DEBUG)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES_WRAP})

target_link_libraries(${PROJECT_NAME} 
    ${QT_LIBRARIES}
    dtkCore)

# Include header files for Visual Studio.
SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Installation
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(FILES ${${PROJECT_NAME}_MODULES} DESTINATION modules)
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
