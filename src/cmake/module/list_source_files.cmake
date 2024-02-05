################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
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

################################################################################
#
# Usage: list_source_files(project_name, directory1, directory2, directory3 ...)
# parse all given directories to list header, source and template path-files
# and them add it to ${project_name}_HEADERS/SOURCES/TEMPLATES.
# a 4th varible is create, ${project_name}_CFILES to embed all the previous ones.
#
################################################################################


foreach(dir ${ARGV})
## #############################################################################
## List all headers and add them to {${project_name}_H
## #############################################################################

  file(GLOB HEADERS
    ${dir}/*.h
    ${dir}/*.hpp
    ${dir}/*.hxx
    )
    
  foreach(header_file ${HEADERS})
      file(RELATIVE_PATH relative_path_to_file ${CMAKE_CURRENT_SOURCE_DIR} ${header_file})
      list(APPEND RELATIVE_${project_name}_HEADERS ${relative_path_to_file})
  endforeach()  
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
    ${dir}/*.tpp
    )
    
  set(${project_name}_TEMPLATES
    ${TEMPLATES}
    ${${project_name}_TEMPLATES}
    )

## #############################################################################
## List all ressources files and add them to {${project_name}_QRC
## #############################################################################

  file(GLOB QRC
    ${dir}/*.qrc
    )
  set(${project_name}_QRC
    ${QRC}
    ${${project_name}_QRC}
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

if (WIN32)
  if(${project_name}_HEADERS)
	list(REMOVE_DUPLICATES ${project_name}_HEADERS)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${${project_name}_HEADERS})
  endif()

  if(${project_name}_SOURCES)
	list(REMOVE_DUPLICATES ${project_name}_SOURCES)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${${project_name}_SOURCES})
  endif()

  if(${project_name}_TEMPLATES)
	list(REMOVE_DUPLICATES ${project_name}_TEMPLATES)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Template Files" FILES ${${project_name}_TEMPLATES})
  endif()
endif()

list(REMOVE_DUPLICATES ${project_name}_CFILES)

endmacro()



macro(to_flat files_list)
    foreach(a_path ${ARGN})
        cmake_path(GET a_path FILENAME a_file_name)
        list(APPEND ${files_list} ${a_file_name})
    endforeach()
    list(REMOVE_DUPLICATES ${files_list})
endmacro()



macro(to_relative relative_path_list base_dir)
    foreach(a_path ${ARGN})
        cmake_path(RELATIVE_PATH relative_path_to_file ${base_dir} a_path)
        list(APPEND ${relative_path_list} ${relative_path_to_file})
    endforeach()
    list(REMOVE_DUPLICATES ${relative_path_list})
endmacro()



macro(to_full full_path_list base_dir)
    foreach(relative_path_to_file ${ARGN})
        cmake_path(SET a_path NORMALIZE "${base_dir}/${relative_path_to_file}")
        list(APPEND ${full_path_list} ${a_path})
    endforeach()
    list(REMOVE_DUPLICATES ${full_path_list})
endmacro()