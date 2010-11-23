project(vtkDataMeshWriterPlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION_MAJOR 0 CACHE STRING "${PROJECT_NAME} major version number.")
set(${PROJECT_NAME}_VERSION_MINOR 1 CACHE STRING "${PROJECT_NAME} minor version number.")
set(${PROJECT_NAME}_VERSION_BUILD 0 CACHE STRING "${PROJECT_NAME} build version number.")
set(${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(VTK)
include(${VTK_USE_FILE})


set(${PROJECT_NAME}_HEADERS_MOC
  vtkDataMeshWriterPlugin.h
#  vtkDataMeshWriterCommand.h
  vtkDataMeshWriter.h
  )

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})
  
set(${PROJECT_NAME}_SOURCES
  vtkDataMeshWriterPlugin.cpp
#  vtkDataMeshWriterCommand.cxx
  vtkDataMeshWriter.cpp
  )

# Include header files for Visual Studio.
SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_MOC} 
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore
  vtkCommon
  vtkFiltering
  vtkIO
)

## #################################################################
## Install rules
## #################################################################
if( VTK_BUILD_SHARED )
  install(TARGETS ${PROJECT_NAME}  
    RUNTIME DESTINATION plugins
    LIBRARY DESTINATION plugins
    ARCHIVE DESTINATION plugins)
  install(DIRECTORY ${VTK_LIBRARY_DIRS}/ 
    DESTINATION lib
    FILES_MATCHING PATTERN "lib*")
else( VTK_BUILD_SHARED )
  install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION plugins
    LIBRARY DESTINATION plugins
    ARCHIVE DESTINATION plugins)
endif( VTK_BUILD_SHARED )


