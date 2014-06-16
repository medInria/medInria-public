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

macro(list_source_files
  project_name
  directories
  )
  
foreach(dir ${ARGV})
## #############################################################################
## List all headers and add them to {${project_name}_H
## #############################################################################

  file(GLOB HEADERS
    ${dir}/*.h
	${dir}/*.hpp
	${dir}/*.hxx
    )
  set(${project_name}_HEADERS
    ${HEADERS}
    ${${project_name}_HEADERS}
    )


## #############################################################################
## List all sources and add them to {${project_name}_SOURCES
## #############################################################################

  file(GLOB SOURCES
    ${dir}/*.cxx
    ${dir}/*.cpp
    )
  set(${project_name}_SOURCES
    ${SOURCES}
    ${${project_name}_SOURCES}
    )
	
	
## #############################################################################
## List all templated files and add them to {${project_name}_TEMPLATE
## #############################################################################

  file(GLOB TEMPLATES
    ${dir}/*.txx
    )
  set(${project_name}_TEMPLATES
    ${SOURCES}
    ${${project_name}_TEMPLATES}
    )

	
## #############################################################################
## List all c++ files
## #############################################################################

  set(${project_name}_CFILES
    ${${project_name}_HEADERS}
	${${project_name}_SOURCES}
    ${${project_name}_TEMPLATES}
    ${${project_name}_CFILES}
    )
    
endforeach()

endmacro()
