FUNCTION (GenerateConfigFile ConfigName)

    #   Set directories for config file

    SET(SphericalHarmonicsVisu_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)
    SET(SphericalHarmonicsVisu_SOURCE_DIRECTORIES  ${CMAKE_CURRENT_SOURCE_DIR})
    SET(SphericalHarmonicsVisu_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR})
    SET(SphericalHarmonicsVisu_LIBRARIES           ${ConfigName} ${ARGN})
    SET(SphericalHarmonicsVisu_USE_FILE            ${CMAKE_CURRENT_BINARY_DIR}/cmake/Use${ConfigName}.cmake)

    #   Creating files for companion projects

    #   Copy the UseSphericalHarmonicsVisu.cmake file to the binary tree for backward compatability.

    CONFIGURE_FILE(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/UseSphericalHarmonicsVisu.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}cmake/Use${ConfigName}.cmake @ONLY IMMEDIATE)

    #   The "use" file.

    SET(${ConfigName}_USE_FILE    ${${ConfigName}_BINARY_DIR}/cmake/UseSphericalFunctionSHVisualization.cmake)
    SET(${ConfigName}_CONFIG_FILE ${${ConfigName}_BINARY_DIR}/cmake/SphericalFunctionSHVisualizationConfig.cmake)

    #   Configure VtkToolsConfig.cmake for the build tree.

    CONFIGURE_FILE(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SphericalHarmonicsVisuConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/${ConfigName}Config.cmake @ONLY IMMEDIATE)

ENDFUNCTION()
