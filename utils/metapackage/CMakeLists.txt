#######################################################################
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
#######################################################################

cmake_minimum_required(VERSION 2.8.4) 

project(medInria-all)
if (UNIX AND NOT APPLE)
  set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
  
  set(CPACK_PACKAGE_NAME "medInria-all")
  set(CPACK_PACKAGE_VENDOR "Inria")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "medInria meta package.")

  set(CPACK_PACKAGE_VERSION_MAJOR "2")
  set(CPACK_PACKAGE_VERSION_MINOR "0")
  set(CPACK_PACKAGE_VERSION_PATCH "1")
  set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

  #set(CPACK_SYSTEM_NAME "i386")
  #set(CPACK_GENERATOR "DEB")

  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sergio Medina")
  set(CPACK_PACKAGE_CONTACT "sergio.medina@inria.fr")

  # runtime dependencies: libopenmpi1.3, libqt4-sql-sqlite
  # vistal dependencies: libxml++1.0c2a, libboost-thread1.40.0
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "dcmtk-inria (>= 3.6.1), dtk (>= 0.6.1), itk-inria-build (> 3.20.1), medinria (>= 2.0.1), medinria-plugins (>= 2.0.1), medinria-plugins-asclepios (>= 2.0.1), medinria-visages-plugins (>= 2.0.1), mips (>= 0.2.0), qtdcm-dev (>= 2.0.2), qtshanoir-dev (>= 0.1.0), ttk-private (>= 1.4.0), vistal (>= 1.1.1), vtk-inria-build (>= 5.8.0), vtkinria3d (>= 1.1.0), libopenmpi1.3, libqt4-sql-sqlite, libxml++1.0c2a, libboost-all-dev, nvidia-settings")

  #RPM dependencies:
  set(CPACK_RPM_PACKAGE_REQUIRES "dcmtk-inria >= 3.6.1, dtk >= 0.6.1, itk-inria-build >= 3.20.1, medinria >= 2.0.1, medinria-plugins >= 2.0.1, medinria-plugins-asclepios >= 2.0.1, medinria-visages-plugins >= 2.0.1, mips >= 0.2.0, qtdcm-dev >= 2.0.2, qtshanoir-dev >= 0.1.0, ttk-private >= 1.4.0, vistal >= 1.1.1, vtk-inria-build >= 5.8.0, vtkinria3d >= 1.1.0")

  set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/postinst;${CMAKE_SOURCE_DIR}/prerm;")
  set (CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/postinst")
  set (CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/prerm")


  if (CPACK_GENERATOR STREQUAL "DEB")
    set(CPACK_SET_DESTDIR "ON")
  endif()
 
  set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/Description.txt")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/Readme.txt")
  set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/Welcome.txt")


  #GET distribution id
  execute_process(COMMAND lsb_release -irs
    COMMAND sed "s/ //"
    COMMAND sed "s/Fedora/fc/"
    COMMAND tr -d '\n' # In Ubuntu the string is Ubuntu\n10.04\n
    OUTPUT_VARIABLE DISTRIB
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND arch 
    OUTPUT_VARIABLE ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${DISTRIB}-${ARCH}")

  include(CPack)

endif()

