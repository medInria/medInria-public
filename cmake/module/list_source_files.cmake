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

macro(list_source_files
  project_name
  directories
  )

foreach(dir ${ARGV})
  
  file(GLOB HEADERS
    ${dir}/*.h
    )
  set(${project_name}_H
    ${HEADERS}
    ${${project_name}_H}
    )

  file(GLOB SOURCES
    ${dir}/*.cxx
    ${dir}/*.cpp
    ${dir}/*.txx
    ${dir}/*.hxx
    ${dir}/*.hpp
    ${dir}/*.h
    )
  set(${project_name}_SOURCES
    ${SOURCES}
    ${${project_name}_SOURCES}
    )
    
endforeach()

endmacro()
