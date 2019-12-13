##############################################################################
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

set(DISTRIB windows)
set(CPACK_PACKAGE_TYPE NSIS)
set(CPACK_PACKAGING_INSTALL_PREFIX "")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_MONOLITHIC_INSTALL 1)

if(CMAKE_CL_64)
	SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
	#  - Text used in the installer GUI
	SET(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} (Win64)")
	#  - Registry key used to store info about the installation
	SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION} (Win64)")
	SET(MSVC_ARCH x64)

else()
    SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
	SET(CPACK_NSIS_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
	SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
	SET(MSVC_ARCH x86)
endif()

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${MSVC_ARCH}")

set(ICON_PATH "${PROJECT_SOURCE_DIR}/src/app/medInria/resources/medInria.ico")

# The icon to install the application.
set(CPACK_NSIS_MUI_ICON ${ICON_PATH})

# The icon to uninstall the application.
set(CPACK_NSIS_MUI_UNIICON ${ICON_PATH})

# Add a desktop shortcut
set(CPACK_CREATE_DESKTOP_LINKS "medInria")

# The icon in the Add/Remove control panel
set(CPACK_NSIS_INSTALLED_ICON_NAME bin\\\\medInria.exe)

# Add medinria to the PATH
set(CPACK_NSIS_MODIFY_PATH "ON")

# Add shortcut in the Startup menu
set(CPACK_PACKAGE_EXECUTABLES "medInria" "medInria")

# Add a link to the application website in the Startup menu.
set(CPACK_NSIS_MENU_LINKS "http://med.inria.fr/" "Homepage for medInria") 

# Run medInria after installation
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "medInria.exe")

# Delete the Startup menu link after uninstallation
set(CPACK_NSIS_DELETE_ICONS_EXTRA "
	Delete '\$SMPROGRAMS\\\\$MUI_TEMP\\\\*.*'
")

if (NOT PRIVATE_PLUGINS_DIRS STREQUAL "")
    foreach(pluginpath ${PRIVATE_PLUGINS_DIRS})
        file(TO_CMAKE_PATH ${pluginpath} pluginpath)
# Add an extra slash, otherwise we copy the folder, not its content
        set(pluginpath "${pluginpath}/")
    	message("Adding ${pluginpath} to the private plugins dirs...")
        install(DIRECTORY ${pluginpath} DESTINATION plugins COMPONENT Runtime FILES_MATCHING PATTERN "*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    endforeach()
endif()

if (NOT PRIVATE_PLUGINS_LEGACY_DIRS STREQUAL "")
    foreach(pluginpath ${PRIVATE_PLUGINS_LEGACY_DIRS})
        file(TO_CMAKE_PATH ${pluginpath} pluginpath)
# Add an extra slash, otherwise we copy the folder, not its content
        set(pluginpath "${pluginpath}/")
        message("Adding ${pluginpath} to the private plugins dirs...")
        install(DIRECTORY ${pluginpath} DESTINATION plugins_legacy COMPONENT Runtime FILES_MATCHING PATTERN "*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    endforeach()
endif()

if (NOT EXTERNAL_PROJECT_PLUGINS_LEGACY_DIRS STREQUAL "")
    foreach(pluginpath ${EXTERNAL_PROJECT_PLUGINS_LEGACY_DIRS})
        file(TO_CMAKE_PATH ${pluginpath} pluginpath)
# Add an extra slash, otherwise we copy the folder, not its content
        set(pluginpath "${pluginpath}/")
        message("Adding ${pluginpath} to the external project plugins dirs...")
        install(DIRECTORY ${pluginpath} DESTINATION plugins_legacy COMPONENT Runtime FILES_MATCHING PATTERN "*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    endforeach()
endif()

#${CMAKE_CFG_INTDIR}
set(APP "\${CMAKE_INSTALL_PREFIX}/bin/medInria.exe")
set(QT_BINARY_DIR "${Qt5_DIR}/../../../bin")
set(QT_PLUGINS_DIR "${Qt5_DIR}/../../../plugins")
set(MEDINRIA_FILES "${medInria_DIR}/Release/bin")
set(PLUGINS_FILES "${medInria_DIR}/Release/bin/plugins")
set(PLUGINS_LEGACY_FILES "${medInria_DIR}/Release/bin/plugins_legacy")

list(APPEND 
  libSearchDirs 
  ${QT_PLUGINS_DIR}/* ${QT_BINARY_DIR} 
  ${ITK_DIR}/bin/Release 
  ${DCMTK_DIR}/bin/Release 
  ${VTK_DIR}/bin/Release 
  ${QtDCM_DIR}/bin/Release 
  ${TTK_DIR}/bin/Release 
  ${dtk_DIR}/bin/Release 
  ${RPI_DIR}/bin/Release 
  ${CMAKE_INSTALL_PREFIX}/bin
  )
  
#set(PLUGINS "\${CMAKE_INSTALL_PREFIX}/plugins/")
#set(PLUGINS_LEGACY "\${CMAKE_INSTALL_PREFIX}/plugins_legacy/")

install(CODE "
file(INSTALL ${MEDINRIA_FILES}/                       DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/            FILES_MATCHING PATTERN \"*${CMAKE_EXECUTABLE_SUFFIX}\")
file(INSTALL ${MEDINRIA_FILES}/                       DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/            FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
#file(INSTALL ${PLUGINS_FILES}/                       DESTINATION \${CMAKE_INSTALL_PREFIX}/plugins/        FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
#file(INSTALL ${PLUGINS_LEGACY_FILES}/                DESTINATION \${CMAKE_INSTALL_PREFIX}/plugins_legacy/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/imageformats/          DESTINATION \${CMAKE_INSTALL_PREFIX}/bin             FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/platforms/             DESTINATION \${CMAKE_INSTALL_PREFIX}/bin             FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/sqldrivers/qsqlite.dll DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/sqldrivers/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_BINARY_DIR}/Qt5Svg.dll              DESTINATION \${CMAKE_INSTALL_PREFIX}/bin             FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QtDCM_DIR}/bin/Release/qtdcm.dll       DESTINATION \${CMAKE_INSTALL_PREFIX}/bin             FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")

file(GLOB_RECURSE PLUGINS
    \${CMAKE_INSTALL_PREFIX}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}
    \${CMAKE_INSTALL_PREFIX}/plugins_legacy/*${CMAKE_SHARED_LIBRARY_SUFFIX}\)
include(BundleUtilities)
fixup_bundle(\"${APP}\"   \"\${PLUGINS}\"   \"${libSearchDirs}\")
" COMPONENT Runtime)
