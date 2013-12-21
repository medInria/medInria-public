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

project(medInria-doc)

find_package( Doxygen QUIET)

if(DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND)

  option(DOC_COMPRESSED_IN_TARGZ
    "Compress archive documentation in a tar.gz" 
    ON
    )
  
  option(DOC_PARSE_ALL
    "Documentation will be generated as weel for applications and plgins as for public libraries"
    OFF
    )  
  mark_as_advanced(DOC_PARSE_ALL)
  
  set(DOC_DIR_TO_PARSE
    "${CMAKE_SOURCE_DIR}/src"
    )  
    
  if(DOC_PARSE_ALL)
    set(DOC_DIR_TO_PARSE
      "${CMAKE_SOURCE_DIR}"
      )
  endif()
   
  set(DOC_ARCHIVES_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
  set(DOC_HTML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc/html/index.html)
  
  configure_file( Doxyfile.txt.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.txt
    )
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_doc.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/generate_doc.cmake
    @ONLY
    ) 
    
  # Run doxygen as a custom command driven by a custom target.
  add_custom_command( OUTPUT ${DOC_HTML_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/img ${CMAKE_CURRENT_BINARY_DIR}/html/img
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/generate_doc.cmake
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/generate_doc.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.txt"
    )

  add_custom_target(doc DEPENDS ${DOC_HTML_OUTPUT_DIR})
  set_target_properties(doc PROPERTIES LABELS Documentation)

else()
  message( WARNING
    "Warning: Doxygen or Dot (Graphviz) not found - Documentation will not be created"
    )

endif()
