find_package( Doxygen QUIET)

# inpired by work done on the CTK and the dtk source.

if ( DOXYGEN_FOUND )
  if ( DOXYGEN_DOT_FOUND )

    # Automatically generate documentation at build time
    set(all_arg)
    if (DOCUMENTATION_TARGET_IN_ALL)
      set(all_arg "ALL")
    endif()

    find_package(Qt4 4.6.0 REQUIRED)
    include(${QT_USE_FILE})

    #
    # Documentation
    #
    set( DOCUMENTATION_HTML_TARZ TRUE)
    set( DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    configure_file( Doxyfile.txt.in
      ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.txt
      )

    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    #message ("cmake revision?: " ${GIT_SHA1})

    configure_file( 
      ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenFooter.html.in
      ${CMAKE_CURRENT_BINARY_DIR}/DoxygenFooter.html
      )

    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/generate_doc.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/generate_doc.cmake
      @ONLY
      )
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/qhc/medInria-plugins.qhcp.in 
      ${CMAKE_CURRENT_BINARY_DIR}/medInria-plugins.qhcp IMMEDIATE @ONLY)
 

    set(doc_output "${CMAKE_CURRENT_BINARY_DIR}/doc/html/index.html")

    # Run doxygen as a custom command driven by a custom target.
    add_custom_command(
      OUTPUT ${doc_output}
      #COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenHeader.html ${CMAKE_CURRENT_BINARY_DIR}/DoxygenHeader.html
      #COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenFooter.html ${CMAKE_CURRENT_BINARY_DIR}/DoxygenFooter.html
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/img      ${CMAKE_CURRENT_BINARY_DIR}/html/img

      COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/generate_doc.cmake"
      COMMAND ${QT_BINARY_DIR}/qhelpgenerator html/index.qhp -o medInria-plugins.qch
      COMMAND ${QT_QCOLLECTIONGENERATOR_EXECUTABLE} medInria-plugins.qhcp -o medInria-plugins.qhc

      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/generate_doc.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.txt"
              "${CMAKE_CURRENT_BINARY_DIR}/DoxygenFooter.html"
              "${CMAKE_CURRENT_BINARY_DIR}/medInria-plugins.qhcp"
      )

    add_custom_target(${PROJECT_NAME}-doc ${all_arg} DEPENDS ${doc_output})
    set_target_properties(${PROJECT_NAME}-doc PROPERTIES LABELS Documentation)
 
  else()
    message( WARNING
      "Warning: Dot not found - Documentation will not be created"
      )
  endif()
 else()
  message(WARNING
     "Warning: Doxygen not found - Documentation will not be created")
endif()
