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
#set(CPACK_MONOLITHIC_INSTALL 1)
set(CPACK_NSIS_COMPONENT_INSTALL 1)

SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
SET(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}") #  - Text used in the installer GUI
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}") #  - Registry key used to store info about the installation
SET(MSVC_ARCH x64)

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
set(CPACK_NSIS_MENU_LINKS "http://med.inria.fr/" "Homepage for medInria" "medDevEnv" ".\\sdk\med_Dev.bat") 

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
    	message("Adding ${pluginpath} to the plugins dirs...")
        install(DIRECTORY ${pluginpath} DESTINATION bin/plugins COMPONENT Runtime FILES_MATCHING PATTERN "*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    endforeach()
endif()

if (NOT PRIVATE_PLUGINS_LEGACY_DIRS STREQUAL "")
    foreach(pluginpath ${PRIVATE_PLUGINS_LEGACY_DIRS})
        file(TO_CMAKE_PATH ${pluginpath} pluginpath)
# Add an extra slash, otherwise we copy the folder, not its content
        set(pluginpath "${pluginpath}/")
        message("Adding ${pluginpath} to the dirs...")
        install(DIRECTORY ${pluginpath} DESTINATION bin/plugins_legacy COMPONENT Runtime FILES_MATCHING PATTERN "*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    endforeach()
endif()


if (NOT EXTERNAL_PROJECT_PLUGINS_LEGACY_DIRS STREQUAL "")
    foreach(pluginpath ${EXTERNAL_PROJECT_PLUGINS_LEGACY_DIRS})
        file(TO_CMAKE_PATH ${pluginpath} pluginpath)
# Add an extra slash, otherwise we copy the folder, not its content
        set(pluginpath "${pluginpath}/")
        message("Adding ${pluginpath} to the plugins dirs...")
        install(DIRECTORY ${pluginpath} DESTINATION bin/plugins_legacy COMPONENT Runtime FILES_MATCHING PATTERN "*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    endforeach()
endif()


set(CONFIG_MODE $<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>$<$<CONFIG:MinSizeRel>:MinSizeRel>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>)
 

set(APP "\${CMAKE_INSTALL_PREFIX}/bin/medInria.exe")
set(QT_BINARY_DIR "${Qt5_DIR}/../../../bin")
set(QT_PLUGINS_DIR "${Qt5_DIR}/../../../plugins")
set(MEDINRIA_FILES "${medInria_DIR}/bin")

list(APPEND 
  libSearchDirs 
  ${QT_PLUGINS_DIR}/imageformats
  ${QT_PLUGINS_DIR}/platforms
  ${QT_BINARY_DIR}/sqldrivers
  ${QT_BINARY_DIR}
  ${TTK_ROOT}/bin
  ${ITK_ROOT}/bin/${CONFIG_MODE}
  ${VTK_ROOT}/bin/${CONFIG_MODE} 
  ${dtk_ROOT}/bin/${CONFIG_MODE} 
  ${RPI_ROOT}/bin/${CONFIG_MODE} 
  ${DCMTK_ROOT}/bin/${CONFIG_MODE}
  ${QtDCM_ROOT}/bin/${CONFIG_MODE} 
  )

install(CODE "

file(GLOB_RECURSE itk_files LIST_DIRECTORIES true \"${ITK_ROOT}/bin/*.dll\")
file(GLOB_RECURSE vtk_files LIST_DIRECTORIES true \"${VTK_ROOT}/bin/*.dll\")
file(GLOB_RECURSE dtk_files LIST_DIRECTORIES true \"${dtk_ROOT}/bin/*.dll\")
file(GLOB_RECURSE ttk_files LIST_DIRECTORIES true \"${TTK_ROOT}/bin/*.dll\")
file(GLOB_RECURSE dcm_files LIST_DIRECTORIES true \"${QtDCM_ROOT}/bin/*.dll\")
file(GLOB_RECURSE qt5_files LIST_DIRECTORIES true \"${QT_BINARY_DIR}/*.dll\")
list(APPEND files \${itk_files})
list(APPEND files \${vtk_files})
list(APPEND files \${dtk_files})
list(APPEND files \${ttk_files})
list(APPEND files \${dcm_files})
list(APPEND files \${qt5_files})

foreach(file \${files})
  get_filename_component(file2delete \${file} NAME)
  if(EXISTS \"${MEDINRIA_FILES}/\${file2delete}\")
    file(REMOVE \"${MEDINRIA_FILES}/\${file2delete}\")
  endif()
endforeach()

file(INSTALL ${MEDINRIA_FILES}/                         DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/              FILES_MATCHING PATTERN \"*${CMAKE_EXECUTABLE_SUFFIX}\")
file(INSTALL ${MEDINRIA_FILES}/                         DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/              FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/imageformats/qgif.dll    DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/imageformats/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/imageformats/qicns.dll   DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/imageformats/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/imageformats/qico.dll    DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/imageformats/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/imageformats/qjpeg.dll   DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/imageformats/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/imageformats/qsvg.dll    DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/imageformats/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/imageformats/qtga.dll    DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/imageformats/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\") #is it really used
file(INSTALL ${QT_PLUGINS_DIR}/imageformats/qtiff.dll   DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/imageformats/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/imageformats/qwbmp.dll   DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/imageformats/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\") #is it really used
file(INSTALL ${QT_PLUGINS_DIR}/imageformats/qwebp.dll   DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/imageformats/ FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\") #is it really used
file(INSTALL ${QT_PLUGINS_DIR}/platforms/qdirect2d.dll  DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/platforms/    FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\") #is it really used
file(INSTALL ${QT_PLUGINS_DIR}/platforms/qminimal.dll   DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/platforms/    FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/platforms/qoffscreen.dll DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/platforms/    FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\") #is it really used
file(INSTALL ${QT_PLUGINS_DIR}/platforms/qwindows.dll   DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/platforms/    FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(INSTALL ${QT_PLUGINS_DIR}/sqldrivers/qsqlite.dll   DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/sqldrivers/   FILES_MATCHING PATTERN \"*${CMAKE_SHARED_LIBRARY_SUFFIX}\")

file(GLOB_RECURSE MED_LIBRARIES
    \${CMAKE_INSTALL_PREFIX}/bin/*${CMAKE_SHARED_LIBRARY_SUFFIX}
    \${CMAKE_INSTALL_PREFIX}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}
    \${CMAKE_INSTALL_PREFIX}/plugins_legacy/*${CMAKE_SHARED_LIBRARY_SUFFIX}\)
include(BundleUtilities)
fixup_bundle(\"${APP}\"   \"\${MED_LIBRARIES}\"   \"${libSearchDirs};\${CMAKE_INSTALL_PREFIX}/bin\")
" COMPONENT Runtime)

if(${SDK_PACKAGING} )
    if(EXISTS ${SDK_DIR} )
        INSTALL(DIRECTORY ${SDK_DIR} DESTINATION . COMPONENT Dev)
        file(INSTALL      ${CMAKE_SOURCE_DIR}/packaging/windows/med_Dev.bat DESTINATION ./sdk/ COMPONENT Dev)
    else()
        message("No folder  ${SDK_DIR} exists. SDK will not be installed.")
    endif()
endif()