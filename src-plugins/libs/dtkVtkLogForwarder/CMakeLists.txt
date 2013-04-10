project(dtkVtkLogForwarder)

# find dtk
find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

# find vtk
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(DTKLOGFORWARDER_HEADERS
  vtkLogForwarder.h
  dtkVtkLogForwarderExport.h)

set(DTKLOGFORWARDER_SOURCES
  vtkLogForwarder.cpp)

add_library(${PROJECT_NAME} SHARED
  ${DTKLOGFORWARDER_HEADERS}
  ${DTKLOGFORWARDER_SOURCES})

target_link_libraries(${PROJECT_NAME}
  dtkCore dtkLog
  vtkCommon
  ${QT_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
