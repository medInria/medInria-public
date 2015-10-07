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


set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt4-sql-sqlite, libqt4-webkit")

# TODO: Remove this when we will not support old ubuntu distribs.

set(CPACK_DEBIAN_PACKAGE_REPLACES 
    "dcmtk-inria, itk-inria-build, medinria-plugins, medinria-plugins-asclepios, medinria-visages-plugins, vtk-inria-build")

set(CPACK_DEBIAN_PACKAGE_HOMEPAGE http://med.inria.fr/)
set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_PROVIDES ${CPACK_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA ${CMAKE_BINARY_DIR}/packaging/linux/prerm;${CMAKE_BINARY_DIR}/packaging/linux/postinst)
