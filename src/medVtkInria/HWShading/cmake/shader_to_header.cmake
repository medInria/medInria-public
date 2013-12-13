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

macro(shader_to_header shaderFile headerName)

file(READ ${shaderFile} shaderContents)

string(REGEX REPLACE
  "\n"
  "\\\\n"
  shaderContents 
  ${shaderContents}
  )

string(REGEX REPLACE
  "\r"
  "\\\\r"
  shaderContents 
  ${shaderContents}
  )
  
# Construct the file in a temporary location, 
# compare with existing and copy if different.
set(tmpShader ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeTmp/shaderToHeader.tmp)

file(WRITE ${tmpShader} "#pragma once\n\n")
file(APPEND ${tmpShader} "#define ${headerName} \"${shaderContents}\"")

# Copy fresh header file only if contents differ from a previously 
# generated version.
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different 
  ${tmpShader}
  ${${PROJECT_NAME}_BINARY_DIR}/shaders/${headerName}.h
  )
endmacro()
