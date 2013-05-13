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

MACRO(SHADER_TO_HEADER ShaderName ShaderFile HeaderFile)

  FILE (READ ${ShaderFile} FILE_CONTENTS)

  STRING(REGEX REPLACE
   "\n"
    "\\\\n"
    FILE_CONTENTS "${FILE_CONTENTS}"
  )

  STRING (REGEX REPLACE
    "\r"
    "\\\\r"
    FILE_CONTENTS "${FILE_CONTENTS}"
  )

  # Construct the file in a temporary location, compare with existing and copy if different.
  SET(TemporaryFile ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeTmp/ShadToHead.tmp)

  SET(OUTPUT_FILE ${TemporaryFile})
  SET(DEFINE_NAME  "_${ShaderName}_h_")
  STRING(REGEX REPLACE "/" "_" DEFINE_NAME ${DEFINE_NAME})
  STRING(REGEX REPLACE "\\." "_" DEFINE_NAME ${DEFINE_NAME})
  STRING(REGEX REPLACE ":" "_" DEFINE_NAME ${DEFINE_NAME})

  FILE(WRITE ${OUTPUT_FILE} "#ifndef bmia_${DEFINE_NAME}\n")
  FILE(APPEND ${OUTPUT_FILE} "#define bmia_${DEFINE_NAME}\n\n")

  FILE(APPEND ${OUTPUT_FILE} "#define ${ShaderName} \"${FILE_CONTENTS}\"\n\n")

  FILE(APPEND ${OUTPUT_FILE} "#endif // bmia_${DEFINE_NAME}\n")

  # Copy fresh header file only if contents differ from a previously generated version.
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${TemporaryFile}" "${HeaderFile}")

ENDMACRO()
