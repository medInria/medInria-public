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

  foreach(library ${DCMTK_LIBRARIES})
    get_filename_component(lib ${library} NAME_WE)
    # TODO: find a way to do this properly 
    set(lib "${lib}.12.dylib")
    add_custom_command(TARGET ${lib_name}
      POST_BUILD
      COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change ${lib} ${DCMTK_DIR}/lib/${lib} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${lib_name}.dylib
      WORKING_DIRECTORY ${CMAKE_CURRENT_BIN_DIR}
    )
  endforeach()

endmacro(FixDCMTKMacLink)
