
ADD_LIBRARY(SphericalHarmonicsVisu ${LIB_PREFIX}
vtkSphereTesselator.cpp
vtkSphericalHarmonicSource.cpp
vtkSphericalHarmonicGlyph.cpp 
itkSphericalHarmonicITKToVTKFilter.cxx
)

TARGET_LINK_LIBRARIES(SphericalHarmonicsVisu
vtkRendering
vtkGraphics
vtkHybrid
vtkIO
vtkCommon
)

INSTALL(TARGETS
        SphericalHarmonicsVisu
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
