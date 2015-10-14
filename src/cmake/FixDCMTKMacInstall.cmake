################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

macro(FixDCMTKMacInstall)
    set(dcmtkLibs 
    charls
    cmr
    dcmdata
    dcmdsig
    dcmfg
    dcmimage
    dcmimgle
    dcmiod
    dcmjpeg
    dcmjpls
    dcmnet
    dcmpstat
    dcmqrdb
    dcmrt
    dcmseg
    dcmsr
    dcmtls
    dcmwlm
    i2d
    ijg12
    ijg16
    ijg8
    oflog
    ofstd
    )

  foreach(lib ${dcmtkLibs})
    if (EXISTS ${DCMTK_DIR}/../../lib${lib}.7.dylib)
      foreach(linkedlib ${dcmtkLibs})
        if (EXISTS ${DCMTK_DIR}/../../lib${linkedlib}.7.dylib)
          execute_process(COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change lib${linkedlib}.7.dylib ${DCMTK_DIR}/../../lib${linkedlib}.7.dylib ${DCMTK_DIR}/../../lib${lib}.7.dylib)
        endif()
      endforeach()
    endif()
  endforeach()
endmacro(FixDCMTKMacInstall)
