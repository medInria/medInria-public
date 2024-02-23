#[=[
Install a new target for library or an executable (not a plugin).

---

Synopsis:

    set_lib_install_rules_generic(<target>
        [
            NAMESPACE <namespace> 
            PUBLIC_DEPENDENCIES <public_dependencies>...
            PUBLIC_LINK_LIBRARIES <public_link_libraries>...
            PUBLIC_DEFINITIONS <public_definitions>...
        ]
    )

---


    The created target will be then refered in the cmake environment as `namespace::target` while the lib object is created as `target.[lib/dll/so/a...]`.
for example to link later to the newly created Bar target delacared in the Foo namespace, one would do:
    
    `target_link_libraries(my_target PUBLIC Foo::Bar)`


How dpendencies required by the target are specfied with PUBLIC_DEPENDENCIES and PRIVATE_DEPENDENCIES. It is a list of parameter to pass to the 
`find_package` and `find_dependency` commands. PUBLIC_DEPENDENCIES are added to the `find_package` (call during the invocation of this function)
and `find_dependency` (call during the invocation of the generated ${_target}Config.cmake module). PRIVATE_DEPENDENCIES are only added to the `find_package` command.

Example: 

the command to install a new lib target called MyLib requiring a public dependence to the libs: Qt6::Widgets, Qt6::Graphics and boost::graph could look like:

    ```
    set_lib_install_rules_generic(MyLib 
        NAMESPACE med
        PUBLIC_DEPENDENCIES
            "Qt6 REQUIRED COMPONENTS Widgets Graphics"
            "Boost REQUIRED COMPONENTS graph"
    )
    ```

    /!\ Note the double quotes surrouding each set of parameters of the find_dependency command passed to PUBLIC_DEPENDENCIES.

---

Parameters:
    <target>:                - Name of the target to install.

Optional Parameters
    <namespace>:             - Name of the namespace the target lives in. (default ${PROJECT_NAME})
    <destination>:           - Name of folder where the installation will be done. (default ${PROJECT_VERSION})
    <public_dependencies>:   - A list of args to pass to the `find_package` and `find_dependency` command in the <target>Config.cmake file
    <file_sets>:             - A list of string like this "<fileset_name> <relative_path>.

#]=]


function(set_lib_install_rules_generic TARGET_NAME)

    ############################################################################
    ############################################################################
    
    set(_options "")
    set(_one_value_args 
        TARGET 
        NAMESPACE
        DESTINATION
    )
    set(_multi_value_args
        PUBLIC_DEPENDENCIES
        FILESETS_HEADERS
    )
    
    cmake_parse_arguments("" "${_options}" "${_one_value_args}" "${_multi_value_args}" ${ARGN})
    
    ############################################################################
    ############################################################################
    
    # Check TARGET_NAME argument
    if(NOT DEFINED TARGET_NAME)
        message(FATAL_ERROR "Call to 'set_lib_install_rules_generic' without any TARGET name")
    endif()
    
    # Check NAMESPACE argument
    if(NOT DEFINED _NAMESPACE)
        if(DEFINED _NAMESPACE_KEYWORDS_MISSING_VALUES)
            message(FATAL_ERROR "Call to 'set_lib_install_rules_generic' for ${TARGET_NAME} has no value for specified NAMESPACE parameter")
        endif()
        set(_NAMESPACE ${PROJECT_NAME})
    endif()
    
    string(TOUPPER ${_NAMESPACE} _NAMESPACE_UPPER)
    
    # Check DESTINATION argument
    if(NOT DEFINED _DESTINATION)
        message(FATAL_ERROR "Call to 'set_lib_install_rules_generic' for ${TARGET_NAME} has no value for specified DESTINATION parameter")
    endif()
    
    
    
    # Check PUBLIC_DEPENDENCIES arguments
    if(DEFINED _PUBLIC_DEPENDENCIES_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "Call to 'set_lib_install_rules_generic' for ${TARGET_NAME} has no value for specified PUBLIC_DEPENDENCIES parameter")
    endif()
    
    # Check FILESETS_HEADERS arguments
    if(DEFINED _FILESETS_HEADERS_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "Call to 'set_lib_install_rules_generic' for ${TARGET_NAME} has no value for specified FILESETS_HEADERS parameter")
    endif()
    
    ############################################################################
    ############################################################################
    
    # Add all public files sets to _FILESET_INST_CMD used dy install clause 
    foreach(_SETS ${_FILESETS_HEADERS})
        string(LENGTH    ${_SETS} _set_string_length)
        string(FIND      ${_SETS} " " split_index)
        string(SUBSTRING ${split_index} 0              ${split_index}        _set_name)
        string(SUBSTRING ${split_index} ${split_index} ${_set_string_length} _set_path)
        
        LIST(APPEND _TMP_FILESET_INST_CMD 
                    "FILE_SET"
                    "${_set_name}"
                    "DESTINATION"
                    "include/${_set_path}")   
        LIST(APPEND _FILESET_INST_CMD ${_TMP_FILESET_INST_CMD})
        UNSET(_TMP_FILESET_INST_CMD)
    endforeach()
    
    
    ############################################################################
    ############################################################################
    
    # include required modules
    include(CMakePackageConfigHelpers)
    
    ############################################################################
    ############################################################################
    
    get_property(GENERATOR_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(${GENERATOR_MULTI_CONFIG})
      set_target_properties( ${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${_DESTINATION}/bin)
      set_target_properties( ${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${_DESTINATION}/bin)
      set_target_properties( ${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${_DESTINATION}/bin)
      set_target_properties( ${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${_DESTINATION}/bin)
    
      set_target_properties( ${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${_DESTINATION}/lib)
      set_target_properties( ${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${_DESTINATION}/lib)
      set_target_properties( ${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${_DESTINATION}/lib)
      set_target_properties( ${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${_DESTINATION}/lib)
    
      set_target_properties( ${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG          ${_DESTINATION}/lib)
      set_target_properties( ${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE        ${_DESTINATION}/lib)
      set_target_properties( ${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     ${_DESTINATION}/lib)
      set_target_properties( ${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${_DESTINATION}/lib)
    else()
      set_target_properties( ${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                ${_DESTINATION}/bin)
      set_target_properties( ${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY                ${_DESTINATION}/lib)
      set_target_properties( ${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY                ${_DESTINATION}/lib)
    endif()
    
    ############################################################################
    ############################################################################
    
    
    
    #Prepare the target short name used by ALIAS system below
    if( ${TARGET_NAME} MATCHES "med.*" )
        STRING(SUBSTRING ${TARGET_NAME} 3 -1 TARGET_SHORT_NAME)
    else()
        set(TARGET_SHORT_NAME ${TARGET_NAME})
    endif()
    
    # Create and prepare export ALIAS for the given target
    get_target_property(_TARGET_TYPE ${TARGET_NAME} TYPE)
    if(${_TARGET_TYPE} STREQUAL STATIC_LIBRARY OR ${_TARGET_TYPE} STREQUAL SHARED_LIBRARY OR ${_TARGET_TYPE} STREQUAL INTERFACE_LIBRARY)
        add_library(${_NAMESPACE}::${TARGET_SHORT_NAME} ALIAS ${TARGET_NAME})
        
        set(_ALIAS "
          if(NOT TARGET ${_NAMESPACE}::${TARGET_SHORT_NAME})
            add_library(${_NAMESPACE}::${TARGET_SHORT_NAME} ALIAS ${_NAMESPACE}::${TARGET_NAME})
          endif()
        ")
    elseif( ${_TARGET_TYPE} STREQUAL EXECUTABLE )
        add_executable(${_NAMESPACE}::${TARGET_SHORT_NAME} ALIAS ${TARGET_NAME})
        set(_ALIAS "add_executable(${_NAMESPACE}::${TARGET_SHORT_NAME} ALIAS ${_NAMESPACE}::${TARGET_NAME})")
    endif()
    
    
    
    
    # Main install part for the target
    install(
        TARGETS ${TARGET_NAME}
        EXPORT  ${TARGET_NAME}Targets
    
        RUNTIME   DESTINATION bin
        LIBRARY   DESTINATION lib
        ARCHIVE   DESTINATION lib
        FRAMEWORK DESTINATION lib
        INCLUDES  DESTINATION include
        RESOURCE  DESTINATION resources/${TARGET_NAME}
        PUBLIC_HEADER DESTINATION "include/${TARGET_NAME}"
        ${_FILESET_INST_CMD}
        )
        
        
    # Generate ${TARGET_NAME}ConfigVersion.cmake in build tree
    write_basic_package_version_file(
      "${MEDINRIA_PATH}/lib/cmake/${TARGET_NAME}/${TARGET_NAME}ConfigVersion.cmake"
      VERSION ${medInria_VERSION}
      COMPATIBILITY AnyNewerVersion
    )
    
    
    # Export the target in the build tree so it can be reused as ${_NAMESPACE}::${TARGET_NAME} 
    # later in the current CMake project the same way the installed one would be used.
    export(EXPORT ${TARGET_NAME}Targets
      FILE "${MEDINRIA_PATH}/lib/cmake/${TARGET_NAME}/${TARGET_NAME}Targets.cmake"
      NAMESPACE ${_NAMESPACE}::
    )
    
    
    # Reconstruct the find_dependency string to evaluate in ${TARGET_NAME}Config.cmake
    if(DEFINED _PUBLIC_DEPENDENCIES)
        set(_FIND_DEPENDENCY_CMD "include(CMakeFindDependencyMacro)")
        foreach(_a_dependency ${_PUBLIC_DEPENDENCIES})
            string(PREPEND _a_dependency "find_dependency(")
            string(APPEND _a_dependency ")")
            list(APPEND _FIND_DEPENDENCY_CMD ${_a_dependency})
        endforeach()
        string(JOIN "\n" _FIND_DEPENDENCY_CMD ${_FIND_DEPENDENCY_CMD})
    endif()
    
    
    # Generate ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    set(PACKAGE_INIT "@PACKAGE_INIT@")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/Config.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in @ONLY)
    
    # Generate ${TARGET_NAME}Config.cmake in build tree from ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
      "${MEDINRIA_PATH}/lib/cmake/${TARGET_NAME}/${TARGET_NAME}Config.cmake"
      INSTALL_DESTINATION lib/cmake/${TARGET_NAME}
      NO_SET_AND_CHECK_MACRO
      NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
    
    
    # Install the Target definition file (${TARGET_NAME}Target.cmake)
    set(ConfigPackageLocation lib/cmake/${TARGET_NAME})
    install(
      EXPORT ${TARGET_NAME}Targets
      FILE ${TARGET_NAME}Targets.cmake
      NAMESPACE ${_NAMESPACE}::
      DESTINATION ${ConfigPackageLocation}
    )
    
    
    # Install the Config files (${_target}Config.cmake and ${_target}ConfigVersion.cmake)
    install(FILES
      ${MEDINRIA_PATH}/lib/cmake/${TARGET_NAME}/${TARGET_NAME}Config.cmake
      ${MEDINRIA_PATH}/lib/cmake/${TARGET_NAME}/${TARGET_NAME}ConfigVersion.cmake
      DESTINATION lib/cmake/${TARGET_NAME}
      )
      
      
      
    ## #############################################################################
    ##  Mac bundle specificites
    ## #############################################################################
    if( ${_TARGET_TYPE} STREQUAL EXECUTABLE )
        if (APPLE)
          set(MACOSX_BUNDLE_BUNDLE_NAME
            ${target}
            )
          set(MACOSX_BUNDLE_ICON_FILE
            ${target}.icns
            )
          set(MACOSX_BUNDLE_SHORT_VERSION_STRING
            ${${target}_VERSION}
            )
          set(MACOSX_BUNDLE_BUNDLE_VERSION
            ${${target}_VERSION}
            )
          set(MACOSX_BUNDLE_LONG_VERSION_STRING
            "Version ${${target}_VERSION}"
            )
    
          install(CODE "
          execute_process(COMMAND
            \${QT_BINARY_DIR}/macdeployqt \${CMAKE_INSTALL_PREFIX}/bin/${target}.app
            )
          execute_process(COMMAND
            \${dtk_ROOT}/bin/dtkDeploy \${CMAKE_INSTALL_PREFIX}/bin/${target}.app -inject-dir=\${CMAKE_CURRENT_BINARY_DIR}/plugins
            )
          ")
        endif()
    endif()
    
    
endfunction()




















MACRO(set_lib_install_rules)
  set_lib_install_rules_generic(${ARGN})
ENDMACRO()

#MACRO(set_lib_install_rules TARGET_NAME)
#  set_lib_install_rules_generic(${TARGET_NAME} DESTINATION ${MEDINRIA_PATH})
#ENDMACRO()

MACRO(set_lib_install_rules_external TARGET_NAME)
  set_lib_install_rules_generic(${TARGET_NAME} DESTINATION ${MEDINRIA_PATH})
ENDMACRO()
