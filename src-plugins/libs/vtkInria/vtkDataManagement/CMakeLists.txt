project(vtkDataManagement)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

set(vtkDataManagement_SRCS
  vtkDataManagerCallback.cxx
  vtkMetaDataSet.cxx
  vtkMetaImageData.cxx
  vtkMetaSurfaceMesh.cxx
  vtkMetaVolumeMesh.cxx
  vtkDataManager.cxx
  vtkMetaDataSetSequence.cxx
  vtkDiffXMLWriter.cxx
  vtkFiberDataSet.cxx
  vtkXMLFiberDataSetReader.cxx
  vtkXMLFiberDataSetWriter.cxx
)

if (VTK_MAJOR_VERSION GREATER 4)
  if (VTK_MINOR_VERSION GREATER 0)
    set(${PROJECT_NAME}_SRCS
    ${${PROJECT_NAME}_SRCS}
    vtkDataManagerWriter.cxx
    vtkDataManagerReader.cxx
    )
  endif()
endif()

if (VTKINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH 
    "vtkInriaPch/vtkInriaPch.h" "../vtkInriaPch/vtkInriaPch.cpp" 
    ${vtkDataManagement_SRCS})
endif()

# C++ library for the InterfaceManagers
add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SRCS}
  )
  
set(${PROJECT_NAME}_LIBS
  ITKCommon
  vtkCommon
  vtkRendering
  vtkWidgets
  vtkVolumeRendering
  vtkHybrid
  vtkImaging    
  vtkGraphics
  vtkFiltering
  vtkImaging
  vtkIO
  vtkRendering
  itkIOAddOn
  ITKIOReview
  )
    
target_link_libraries(${PROJECT_NAME}
  ${ITK_LIBRARIES}
  ${${PROJECT_NAME}_LIBS}
  )

if ( VTKINRIA_BUILD_TESTING )
  add_subdirectory(Testing)
endif()


