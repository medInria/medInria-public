##############################################################################
#
# medInria
#
# Copyright (c) INRIA 2013-2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

set(CPACK_RPM_PACKAGE_REQUIRES "qt, qtwebkit")
if(${DISTRIBUTOR_ID} MATCHES 14|15)
    set(CPACK_RPM_PACKAGE_REQUIRES "qt, qt-webkit")
endif()

# TODO: Remove this when we will not support old fedora distribs. 

set(CPACK_RPM_PACKAGE_OBSOLETES 
    "dcmtk-inria, itk-inria-build, medinria-plugins, medinria-plugins-asclepios, medinria-visages-plugins, vtk-inria-build")

set(CPACK_RPM_PACKAGE_PROVIDES "${CPACK_PACKAGE_NAME} = ${CPACK_PACKAGE_VERSION}")
set(CPACK_RPM_PACKAGE_LICENSE BSD)
set (CPACK_RPM_PACKAGE_ARCHITECTURE ${ARCH})

set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE  ${PROJECT_BINARY_DIR}/linux/postinst)
set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE ${PROJECT_BINARY_DIR}/linux/prerm)
