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

  foreach(library ${DCMTK_LIBRARIES})
    get_filename_component(lib ${library} NAME_WE)
    if (EXISTS ${DCMTK_DIR}/lib/${lib}.dylib)
      foreach(linkedlibrary ${DCMTK_LIBRARIES})
        get_filename_component(linkedlib ${linkedlibrary} NAME_WE)
        if (EXISTS ${DCMTK_DIR}/lib/${linkedlib}.dylib)
          execute_process(COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change ${linkedlib}.dylib ${DCMTK_DIR}/lib/${linkedlib}.dylib ${DCMTK_DIR}/lib/${lib}.dylib)
        endif()
      endforeach()
    endif()
  endforeach()

endmacro(FixDCMTKMacInstall)
