cmake_minimum_required(VERSION 2.6)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

PROJECT(SphericalHarmonicsVisu)


if(vtkINRIA3D_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(SphericalHarmonicsVisu_SOURCES_PCH
    "vtkInria3dPch/vtkInria3dPch.h" "../vtkInria3dPch/vtkInria3dPch.cpp"
    ${SphericalHarmonicsVisu_SRCS})
endif(vtkINRIA3D_USE_PRECOMPILED_HEADERS)

  # C++ library for the InterfaceManagers
  ADD_LIBRARY(SphericalHarmonicsVisu ${vtkINRIA3D_LIBS_STYLE}
  ${SphericalHarmonicsVisu_SOURCES_PCH}
  ${SphericalHarmonicsVisu_SRCS}
  )

  SET(SphericalHarmonicsVisu_LIBS
    vtkCommon
    vtkRendering
    vtkGraphics
    vtkIO
    vtkHybrid
  )

  TARGET_LINK_LIBRARIES(SphericalHarmonicsVisu
  ${SphericalHarmonicsVisu_LIBS}
  )

  if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
    install(TARGETS SphericalHarmonicsVisu
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

