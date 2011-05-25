project(dtkVtkLogForwarder)

# find dtk
find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

# find vtk
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(DTKLOGFORWARDER_HEADERS
  vtkLogForwarder.h)

set(DTKLOGFORWARDER_SOURCES
  vtkLogForwarder.cpp)

set(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_library(${PROJECT_NAME} 
  ${DTKLOGFORWARDER_HEADERS}
  ${DTKLOGFORWARDER_SOURCES})

target_link_libraries(${PROJECT_NAME}
  dtkCore
  ${VTK_LIBRARIES}  )


