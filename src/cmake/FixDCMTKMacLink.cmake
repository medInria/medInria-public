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

macro(FixDCMTKMacLink lib_name)

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

  set(WORK_DIR "${CMAKE_BINARY_DIR}/lib")
  if (NOT EXISTS "${WORK_DIR}/lib${lib_name}.dylib")
    set(WORK_DIR "${CMAKE_BINARY_DIR}/plugins")
    if (NOT EXISTS "${WORK_DIR}/lib${lib_name}.dylib")
      set(WORK_DIR "${CMAKE_BINARY_DIR}/plugins_legacy")
    endif()
  endif()

  foreach(lib ${dcmtkLibs})
    if(EXISTS "${DCMTK_DIR}/../../lib${lib}.7.dylib")
      add_custom_command(TARGET ${lib_name}
	      POST_BUILD
              COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change lib${lib}.7.dylib ${DCMTK_DIR}/../../lib${lib}.7.dylib ${WORK_DIR}/lib${lib_name}.dylib
        WORKING_DIRECTORY ${WORK_DIR}
        )
    endif()
  endforeach()

endmacro(FixDCMTKMacLink)
