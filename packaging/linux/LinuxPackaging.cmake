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

# Get distribution name and architecture

execute_process(COMMAND lsb_release -a
                COMMAND grep "^Distributor ID:" 
                COMMAND sed -e "s/Distributor ID:[ \t]*//ig"
                OUTPUT_VARIABLE DISTRIBUTOR_ID
                OUTPUT_STRIP_TRAILING_WHITESPACE)
  
execute_process(COMMAND lsb_release -a
                COMMAND grep "^Release:"
                COMMAND sed -e "s/Release:[ \t]*//ig"
                OUTPUT_VARIABLE RELEASE
                OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND arch 
                OUTPUT_VARIABLE ARCH 
                OUTPUT_STRIP_TRAILING_WHITESPACE)
  
set(CPACK_PACKAGE_FILE_NAME 
    "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${DISTRIBUTOR_ID}_${RELEASE}-${ARCH}")
 
# Set the right package generator
set(CPACK_GENERATOR DEB)
if(${DISTRIBUTOR_ID} MATCHES fc|fedora|Fedora|Centos|centos|SUSE|Suse|suse)
    set(CPACK_GENERATOR RPM)
endif()

set (CPACK_PACKAGING_INSTALL_PREFIX /opt/medInria CACHE STRING "Prefix where the package will be installed")
mark_as_advanced(CPACK_PACKAGING_INSTALL_PREFIX)

# Remember the linux packaging source dir
set(CURRENT_SRC_DIR ${CMAKE_SOURCE_DIR}/packaging/linux)
set(CURRENT_BIN_DIR ${CMAKE_BINARY_DIR}/packaging/linux)

# The postinst and prerm scripts are run at package installation for both, or uninstallation for prerm
configure_file(${CURRENT_SRC_DIR}/prerm.in    ${CURRENT_BIN_DIR}/prerm)
configure_file(${CURRENT_SRC_DIR}/postinst.in ${CURRENT_BIN_DIR}/postinst)

# Include settings specific to DEB and RPM
include(${CURRENT_SRC_DIR}/RPM.cmake)
include(${CURRENT_SRC_DIR}/DEB.cmake)

# Generate desktop file
configure_file(${CURRENT_SRC_DIR}/medInria.desktop.in ${CURRENT_BIN_DIR}/medInria.desktop @ONLY)
install(FILES ${CURRENT_BIN_DIR}/medInria.desktop DESTINATION .)
install(FILES ${CMAKE_SOURCE_DIR}/src/app/medInria/resources/medInria.png DESTINATION .)

# Configure a cleaning script run after the creation of the archive to be packaged
configure_file(${CURRENT_SRC_DIR}/PostArchiveCleanupScript.cmake.in ${CURRENT_BIN_DIR}/PostArchiveCleanupScript.cmake @ONLY)
install(CODE "include(${CURRENT_BIN_DIR}/PostArchiveCleanupScript.cmake)")

# Add project to package

# save the medinria-superproject install target to add it last
set(backup_CPACK_INSTALL_CMAKE_PROJECTS ${CPACK_INSTALL_CMAKE_PROJECTS} ${CMAKE_BINARY_DIR} ${CMAKE_PROJECT_NAME} ALL "/")

#clear it
set(CPACK_INSTALL_CMAKE_PROJECTS "")
foreach(external_project ${external_projects}) 
	if(NOT USE_SYSTEM_${external_project} AND BUILD_SHARED_LIBS_${external_project})
		ExternalProject_Get_Property(${external_project} binary_dir)
		set(CPACK_INSTALL_CMAKE_PROJECTS ${CPACK_INSTALL_CMAKE_PROJECTS} ${binary_dir} ${external_project} ALL "/")
	endif()
endforeach()

# force the medinria-superproject install target to run last so we can use it
# to cleanup
set(CPACK_INSTALL_CMAKE_PROJECTS ${CPACK_INSTALL_CMAKE_PROJECTS} ${backup_CPACK_INSTALL_CMAKE_PROJECTS})
