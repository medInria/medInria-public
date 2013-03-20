project(medInria)

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
    medApplication.h
    medHomepageArea.h
    medHomepageButton.h
    medBrowserArea.h
    medBrowserSourceSelectorToolBox.h
    medMainWindow.h
    medWorkspaceArea.h
    medDiffusionWorkspace.h
    medVisualizationWorkspace.h
    medRegistrationWorkspace.h
    medSegmentationWorkspace.h
    medFilteringWorkspace.h
    medPacsDataSource.h
    medFileSystemDataSource.h
    medDatabaseDataSource.h
    medSaveModifiedDialog.h
    medSplashScreen.h
    medEmptyDbWarning.h
    medPluginWidget.h)

set(${PROJECT_NAME}_HEADERS medWorkspaceArea_p.h ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
    main.cpp
    medApplication.cpp
    medHomepageArea.cpp
    medHomepageButton.cpp
    medBrowserArea.cpp
    medBrowserSourceSelectorToolBox.cpp
    medMainWindow.cpp
    medWorkspaceArea.cpp
    medDiffusionWorkspace.cpp
    medVisualizationWorkspace.cpp
    medRegistrationWorkspace.cpp
    medSegmentationWorkspace.cpp
    medFilteringWorkspace.cpp
    medPacsDataSource.cpp
    medFileSystemDataSource.cpp
    medDatabaseDataSource.cpp
    medSaveModifiedDialog.cpp
    medSplashScreen.cpp
    medEmptyDbWarning.cpp
    medPluginWidget.cpp)

set(${PROJECT_NAME}_RESOURCES medInria.qrc)

# Include header files for Visual Studio.

set(${PROJECT_NAME}_SOURCES
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_HEADERS_MOC}
)

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})
qt4_add_RESOURCES(${PROJECT_NAME}_SOURCES_QRC ${${PROJECT_NAME}_RESOURCES})

set(DEPLOYMENT_SYSTEM)
IF (APPLE)
    set(DEPLOYMENT_SYSTEM MACOSX_BUNDLE)
elseif (WIN32)
    if (NOT ${MED_SHOW_CONSOLE})
        set(DEPLOYMENT_SYSTEM WIN32)
    endif()
    set(${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_SOURCES} ${PROJECT_NAME}.rc)
endif()

if (MEDINRIA_USE_PRECOMPILED_HEADERS)
    add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH
                           "medPch.h" "../../src/medPch/medPch.cpp"
                           ${${PROJECT_NAME}_SOURCES_WRAP}
                           ${${PROJECT_NAME}_SOURCES_QRC}
                           ${${PROJECT_NAME}_SOURCES_MOC}
                           ${${PROJECT_NAME}_SOURCES})
endif()

include_directories(${PROJECT_NAME}_BINARY_DIR)

add_executable(${PROJECT_NAME} ${DEPLOYMENT_SYSTEM}
      ${${PROJECT_NAME}_SOURCES_PCH}
      ${${PROJECT_NAME}_SOURCES_WRAP}
      ${${PROJECT_NAME}_SOURCES_QRC}
      ${${PROJECT_NAME}_SOURCES_MOC}
      ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME}
                      ${QT_LIBRARIES} dtkCore dtkScript dtkGui dtkVr medGui medSql medPacs)

# Include header files for Visual Studio.

SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Bundle setup
## #################################################################

if (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME}.icns)
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING 0.1.0)
    SET(MACOSX_BUNDLE_VERSION 0.1.0)
    SET(MACOSX_BUNDLE_LONG_VERSION_STRING Version 0.1.0)
    set(${PROJECT_NAME}_RESOURCE_DIR ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.app/Contents/Resources)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${${PROJECT_NAME}_RESOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.icns ${${PROJECT_NAME}_RESOURCE_DIR})
endif()

## #################################################################
## Installation
## #################################################################

#   medInria.desktop file is used by Linux xdg compliant systems to add the app to the OS menu

if (UNIX AND NOT APPLE)
    configure_file(medInria.desktop.in medInria.desktop)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/medInria.desktop
            DESTINATION share/applications
            COMPONENT application)
    #   Once the new icon is ready we need to change this
    install(FILES medInria.png
            DESTINATION share/apps/medInria/icons
            COMPONENT application)
endif()

install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION bin
        RUNTIME DESTINATION bin
        COMPONENT application)

if (APPLE)
    install(CODE "execute_process(COMMAND macdeployqt \${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app)")
endif()
