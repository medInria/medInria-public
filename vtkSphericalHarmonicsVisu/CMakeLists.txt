cmake_minimum_required(VERSION 2.6)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

PROJECT(vtkSphericalHarmonicsVisu)

  SET(vtkSphericalHarmonicsVisu_SRCS
vtkSphereTesselator.cpp
vtkSphericalHarmonicSource.cpp
vtkSphericalHarmonicGlyph.cpp
itkSphericalHarmonicITKToVTKFilter.cxx
  )

if(vtkINRIA3D_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(vtkSphericalHarmonicsVisu_SOURCES_PCH
    "vtkInria3dPch/vtkInria3dPch.h" "../vtkInria3dPch/vtkInria3dPch.cpp"
    ${vtkSphericalHarmonicsVisu_SRCS})
endif(vtkINRIA3D_USE_PRECOMPILED_HEADERS)

  # C++ library for the InterfaceManagers
  ADD_LIBRARY(vtkSphericalHarmonicsVisu ${vtkINRIA3D_LIBS_STYLE}
  ${vtkSphericalHarmonicsVisu_SOURCES_PCH}
  ${vtkSphericalHarmonicsVisu_SRCS}
  )

  SET(vtkSphericalHarmonicsVisu_LIBS
    vtkCommon
    vtkRendering
    vtkGraphics
    vtkIO
    vtkHybrid
  )

  TARGET_LINK_LIBRARIES(vtkSphericalHarmonicsVisu
  ${vtkSphericalHarmonicsVisu_LIBS}
  )

  if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
    install(TARGETS vtkSphericalHarmonicsVisu
    LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
    ARCHIVE DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
    )
  endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

  if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
    install(DIRECTORY ./
        DESTINATION ${vtkINRIA3D_INSTALL_INCLUDE_DIR}
        FILES_MATCHING PATTERN "*.h")
  endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)

