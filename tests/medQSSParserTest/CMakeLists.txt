project(medQSSParserTest)

set(${PROJECT_NAME}_HEADERS_MOC
medQSSParserTest.h)

set (${PROJECT_NAME}_SOURCES
medQSSParserTest.cpp)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_executable(${PROJECT_NAME}
${${PROJECT_NAME}_SOURCES_MOC}
${${PROJECT_NAME}_SOURCES}
${${PROJECT_NAME}_HEADERS_MOC}
)

target_link_libraries(${PROJECT_NAME} medCore)

add_test(medQSSParserTest ${CMAKE_BINARY_DIR}/bin/medQSSParserTest)
