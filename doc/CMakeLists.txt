FIND_PACKAGE( Doxygen QUIET)

# inpired by work done on the CTK and the dtk source.

IF( DOXYGEN_FOUND )
  IF( DOXYGEN_DOT_FOUND )

    # Automatically generate documentation at build time
    SET(all_arg)
    IF (DOCUMENTATION_TARGET_IN_ALL)
      SET(all_arg "ALL")
    ENDIF()

    #
    # Documentation
    #
    SET( DOCUMENTATION_HTML_TARZ TRUE)
    SET( DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    CONFIGURE_FILE( Doxyfile.txt.in
      ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.txt
      )

    CONFIGURE_FILE(
      ${CMAKE_CURRENT_SOURCE_DIR}/generate_doc.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/generate_doc.cmake
      @ONLY
      )
    CONFIGURE_FILE(
      ${CMAKE_CURRENT_SOURCE_DIR}/qhc/medinria-plugins.qhcp.in 
      ${CMAKE_CURRENT_BINARY_DIR}/medinria-plugins.qhcp IMMEDIATE @ONLY)
 

    SET(doc_output "${CMAKE_CURRENT_BINARY_DIR}/doc/html/index.html")

    # Run doxygen as a custom command driven by a custom target.
    ADD_CUSTOM_COMMAND(
      OUTPUT ${doc_output}
      #COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenHeader.html ${CMAKE_CURRENT_BINARY_DIR}/DoxygenHeader.html
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenFooter.html ${CMAKE_CURRENT_BINARY_DIR}/DoxygenFooter.html
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/img      ${CMAKE_CURRENT_BINARY_DIR}/html/img

      COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/generate_doc.cmake"
      COMMAND ${QT_BINARY_DIR}/qhelpgenerator html/index.qhp -o medinria-plugins.qch
      COMMAND ${QT_QCOLLECTIONGENERATOR_EXECUTABLE} medinria-plugins.qhcp -o medinria-plugins.qhc

      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/generate_doc.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.txt"
              "${CMAKE_CURRENT_BINARY_DIR}/medinria-plugins.qhcp"
      )

    ADD_CUSTOM_TARGET(doc ${all_arg} DEPENDS ${doc_output})
    SET_TARGET_PROPERTIES(doc PROPERTIES LABELS Documentation)
 
  ELSE( DOXYGEN_DOT_FOUND )
    MESSAGE( WARNING
      "Warning: Dot not found - Documentation will not be created"
      )
  ENDIF( DOXYGEN_DOT_FOUND )
 ELSE( DOXYGEN_FOUND )
  MESSAGE(WARNING
     "Warning: Doxygen not found - Documentation will not be created")
ENDIF( DOXYGEN_FOUND )
