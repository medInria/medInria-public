
INCLUDE_DIRECTORIES("../../src/ITKDCMTKIO")

#if (MEDINRIA_ITKDATAIMAGEREADER_BUILD_DCMTK_SUPPORT)

add_executable(itkDCMTKReadWriteBenchmark
  itkDCMTKReadWriteBenchmark.cxx)
  
target_link_libraries(itkDCMTKReadWriteBenchmark
  ITKDCMTKIO
  ${ITK_LIBRARIES})

add_executable(itkDCMTKConvert
  itkDCMTKConvert.cxx)
  
target_link_libraries(itkDCMTKConvert
  ITKDCMTKIO
  ${ITK_LIBRARIES}
  ${QT_LIBRARIES}
)

#endif(MEDINRIA_ITKDATAIMAGEREADER_BUILD_DCMTK_SUPPORT)