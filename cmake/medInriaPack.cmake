option(ENABLE_PACKAGING "Enable Packaging" ON)

if (ENABLE_PACKAGING)

    include(InstallRequiredSystemLibraries)

    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}")

    if (DISTRIB)
        #   Not sure this is the best naming...
        set(DistribExtension "${DISTRIB}-${PACKAGE_ARCH}")
    else()
        set(DistribExtension "${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-${DistribExtension}")

    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}-src")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} Project")
    set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_NAME})

    set(CPACK_PACKAGE_VENDOR "http://www.inria.fr/")
    set(CPACK_PACKAGE_CONTACT "medinria-userfeedback@inria.fr")
    set(CPACK_PACKAGE_RELOCATABLE TRUE)

    set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README.txt)
    set(CPACK_PACKAGE_VERSION_MAJOR    ${${PROJECT_NAME}_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR    ${${PROJECT_NAME}_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH    ${${PROJECT_NAME}_VERSION_BUILD})
    set(CPACK_RESOURCE_FILE_LICENSE    ${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt)

    set(CPACK_COMPONENTS_ALL application libraries headers devel config tools python doc)

    set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "medInria application")
    set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME   "medInria libraries")
    set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME     "medInria headers")
    set(CPACK_COMPONENT_CONFIG_DISPLAY_NAME      "medInria cmake configuration files")
    set(CPACK_COMPONENT_TOOLS_DISPLAY_NAME       "medInria tools")
    set(CPACK_COMPONENT_PYTHON_DISPLAY_NAME      "medInria python modules")
    set(CPACK_COMPONENT_DOC_DISPLAY_NAME         "medInria documentation")

    set(CPACK_COMPONENT_APPLICATION_DESCRIPTION "medInria applications")
    set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION   "medInria libraries")
    set(CPACK_COMPONENT_HEADERS_DESCRIPTION     "headers for developing with medInria")
    set(CPACK_COMPONENT_CONFIG_DESCRIPTION      "Cmake files to help developing with medInria")
    set(CPACK_COMPONENT_TOOLS_DESCRIPTION       "Tools to help with medInria development")
    set(CPACK_COMPONENT_PYTHON_DESCRIPTION      "Python medInria modules")
    set(CPACK_COMPONENT_DOC_DESCRIPTION         "Documentation for medInria")

    set(CPACK_COMPONENT_APPLICATION_DEPENDS libraries)
    set(CPACK_COMPONENT_HEADERS_DEPENDS     libraries)
    set(CPACK_COMPONENT_CONFIG_DEPENDS      libraries headers)
    set(CPACK_COMPONENT_TOOLS_DEPENDS       libraries)
    set(CPACK_COMPONENT_PYTHON_DEPENDS      libraries)

    set(CPACK_COMPONENT_APPLICATION_GROUP "runtime")
    set(CPACK_COMPONENT_LIBRARIES_GROUP   "runtime")
    set(CPACK_COMPONENT_HEADERS_GROUP     "devel")
    set(CPACK_COMPONENT_CONFIG_GROUP      "devel")
    set(CPACK_COMPONENT_TOOLS_GROUP       "devel")
    set(CPACK_COMPONENT_PYTHON_GROUP      "runtime")
    set(CPACK_COMPONENT_DOC_GROUP         "runtime")

    set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION     "medInria runtime environment")
    set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION "medInria software development kit")

    set(CPACK_BINARY_TGZ ON)

    if ("${CPACK_PACKAGE_TYPE}" STREQUAL "RPM")
        set(CPACK_RPM_COMPONENT_INSTALL ON)
        set(CPACK_BINARY_RPM ON)
        set(CPACK_RPM_PACKAGE_LICENSE "Berkeley")
        set(CPACK_RPM_PACKAGE_GROUP "Applications")
        #set(CPACK_RPM_USER_BINARY_SPECFILE ${CMAKE_CURRENT_SOURCE_DIR}/packaging/medInria.spec.in)
        #set(CPACK_RPM_PACKAGE_DEBUG TRUE)
    endif()

    add_custom_target(packaging
        DEPENDS install
        COMMENT "Build package for ${DISTRIB}")

    if (UNIX AND NOT APPLE)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "dtk (>= 0.6.0)") 
        add_custom_command(TARGET packaging
            COMMAND cpack -G ${CPACK_PACKAGE_TYPE})
    endif()

    IF (WIN32)
        macro(MAKE_NSIS_INSTALLER TARGET INPUT VERSION)
            find_file(MAKENSIS makensis.exe
                "C:/Program Files/NSIS/" 
                "C:/Program Files (x86)/NSIS/" 
                )    
            if (MAKENSIS)       
                file(TO_CMAKE_PATH "$ENV{SYSTEMROOT}" SYSTEMROOT)
                if (CMAKE_CL_64)
                    set(MSVC_ARCH x64)
                    set(PROGFILES_DIR "$PROGRAMFILES64")
                else()
                    set(MSVC_ARCH x86)
                    set(PROGFILES_DIR "$PROGRAMFILES")
                endif()
                set(NSIS_OPTIONS
                    ${NSIS_OPTIONS}
                    "/DmedInriaDIR=${EXECUTABLE_OUTPUT_PATH}"
                    "/DmedInriaLIBDIR=${LIBRARY_OUTPUT_PATH}/release"
                    "/DVERSION=${VERSION}"
                    "/DSRCDIR=${PROJECT_SOURCE_DIR}"
                    "/DINST_PREFIX=${CMAKE_INSTALL_PREFIX}"
                    #must be changed but ${CMAKE_INSTALL_PREFIX} has slashes not backslashes...
                    
                    "/DPACK_INSTALLDIR=${PROGFILES_DIR}\\inria"
                    "/DPROJECT_NAME=${PROJECT_NAME}"
                    "/DMED_EXECUTABLE=${PROJECT_NAME}.exe"
                    "/DMED_INSTALLER_EXE=${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}-win32-${MSVC_ARCH}.exe"
                    )
                add_custom_command(
                    TARGET ${TARGET} POST_BUILD
                    COMMAND ${MAKENSIS} ${NSIS_OPTIONS} ${INPUT})
            ENDIF()
        endmacro()
        
        MAKE_NSIS_INSTALLER(packaging ${PROJECT_SOURCE_DIR}/packaging/installerMedinria.nsi ${${PROJECT_NAME}_VERSION})
    ENDIF()

    include(CPack)

endif()
