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

MACRO(FixDCMTKMacLink lib_name)

  foreach(lib dcmdata dcmimage dcmimgle dcmjpeg dcmnet dcmpstat dcmqrdb dcmsr dcmtls ijg12 ijg16 ijg8 oflog ofstd)
    add_custom_command(TARGET ${lib_name}
	POST_BUILD
	COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change lib${lib}.dylib ${DCMTK_DIR}/lib/lib${lib}.dylib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${lib_name}.dylib
        WORKING_DIRECTORY ${CMAKE_CURRENT_BIN_DIR}
    )
  endforeach()

ENDMACRO(FixDCMTKMacLink)
