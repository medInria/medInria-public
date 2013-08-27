project(medPluginGeneratorTest)

set(${PROJECT_NAME}_HEADERS_MOC
medPluginGeneratorTest.h)

set (${PROJECT_NAME}_SOURCES
medPluginGeneratorTest.cpp)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_executable(${PROJECT_NAME}
${${PROJECT_NAME}_SOURCES_MOC}
${${PROJECT_NAME}_SOURCES}
${${PROJECT_NAME}_HEADERS_MOC}
)

add_definitions("-DCMAKE_DTK_DIR=\"${dtk_DIR}\""
                "-DCMAKE_ITK_DIR=\"${ITK_DIR}\""
                "-DCMAKE_RPI_DIR=\"${RPI_DIR}\""
                "-DCMAKE_BINARY_DIR=\"${CMAKE_BINARY_DIR}\"")

target_link_libraries(${PROJECT_NAME} medCore QtTest)

add_test(medPluginGeneratorTest ${CMAKE_BINARY_DIR}/bin/medPluginGeneratorTest ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeTmp)
