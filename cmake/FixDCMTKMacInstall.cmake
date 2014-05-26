################################################################################
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
################################################################################

macro(FixDCMTKMacInstall)
    set(dcmtkLibs 
    dcmdata 
    dcmimage 
    dcmimgle 
    dcmjpeg 
    dcmnet 
    dcmpstat 
    dcmqrdb 
    dcmsr 
    dcmtls 
    ijg12 
    ijg16 
    ijg8 
    oflog 
    ofstd
    )

  foreach(lib ${dcmtkLibs})
    if (EXISTS ${DCMTK_DIR}/lib/lib${lib}.dylib)
      foreach(linkedlib ${dcmtkLibs})
        if (EXISTS ${DCMTK_DIR}/lib/lib${linkedlib}.dylib)
          execute_process(COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change lib${linkedlib}.dylib ${DCMTK_DIR}/lib/lib${linkedlib}.dylib ${DCMTK_DIR}/lib/lib${lib}.dylib)
        endif()
      endforeach()
    endif()
  endforeach()
endmacro(FixDCMTKMacInstall)
