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

macro(FixDCMTKMacInstall target_libraries dcmtk_libraries dcmtk_dir)
  foreach(targetLib ${target_libraries})
    foreach(linkedlibrary ${dcmtk_libraries})
      get_filename_component(linkedlib ${linkedlibrary} NAME_WE)
      execute_process(COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change ${linkedlib}.12.dylib ${dcmtk_dir}/lib/${linkedlib}.12.dylib ${targetLib})
    endforeach()
  endforeach()

endmacro(FixDCMTKMacInstall)
