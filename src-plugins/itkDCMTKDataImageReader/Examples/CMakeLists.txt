#######################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#######################################################################

include_directories(".." ${PROJECT_SOURCE_DIR}/../src/ITKDCMTKIO/)

#if (MEDINRIA_ITKDATAIMAGEREADER_BUILD_DCMTK_SUPPORT)

add_executable(itkDCMTKReadWriteBenchmark
  itkDCMTKReadWriteBenchmark.cxx)
  
target_link_libraries(itkDCMTKReadWriteBenchmark
  ITKDCMTKIO
  ${ITK_LIBRARIES})

add_executable(itkDCMTKConvert
  itkDCMTKConvert.cxx)
  
find_package(Qt4 4.6.0 REQUIRED)
include(${QT_USE_FILE})

target_link_libraries(itkDCMTKConvert
  ITKDCMTKIO
  ${ITK_LIBRARIES}
  ${QT_LIBRARIES})

#endif()
