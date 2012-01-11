cmake_minimum_required(VERSION 2.8.4) 

project(medinria-all)

set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)

set(CPACK_PACKAGE_NAME "medinria-all")
set(CPACK_PACKAGE_VENDOR "Inria")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MedInria meta package.")

set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

#set(CPACK_SYSTEM_NAME "i386")
#set(CPACK_GENERATOR "DEB")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sergio Medina")
set(CPACK_PACKAGE_CONTACT "sergio.medina@inria.fr")

# runtime dependencies: libopenmpi1.3, libqt4-sql-sqlite
# vistal dependencies: libxml++1.0c2a, libboost-thread1.40.0
set(CPACK_DEBIAN_PACKAGE_DEPENDS "dcmtk-inria (>= 3.6.1), dtk (>= 0.6.0), itk (>= 3.20.0), medinria (>= 2.0.0), medinria-plugins (>= 2.0.0), medinria-plugins-asclepios (>= 2.0.0), medinria-visages-plugins (>= 2.0.0), mips (>= 0.1.1), qtdcm-dev (>= 2.0.0), qtshanoir-dev (>= 0.1.0), ttk-private (>= 1.4.0), vistal (>= 1.1.0), vtk (>= 5.8.0), vtkinria3d (>= 1.0.0), libopenmpi1.3, libqt4-sql-sqlite, libxml++1.0c2a, libboost-dev-all")

#RPM dependencies:
set(CPACK_RPM_PACKAGE_REQUIRES "dcmtk-inria >= 3.6.1, dtk >= 0.6.0, itk >= 3.20.0, medinria >= 2.0.0, medinria-plugins >= 2.0.0, medinria-plugins-asclepios >= 2.0.0, medinria-visages-plugins >= 2.0.0, mips >= 0.1.1, qtdcm-dev >= 2.0-1, qtshanoir-dev >= 0.1.0, ttk-private >= 1.4.0, vistal >= 1.1.0, vtk >= 5.8.0, vtkinria3d >= 1.0.0")

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/postinst;${CMAKE_SOURCE_DIR}/prerm;")
set(CPACK_RPM_PACKAGE_RELOCATABLE OFF)
if (CPACK_GENERATOR STREQUAL "DEB")
  set(CPACK_SET_DESTDIR "ON")
endif()
 
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/Description.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/Readme.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/Welcome.txt")

set(CPACK_PACKAGE_FILE_NAME "medinria-all-${CPACK_PACKAGE_VERSION}")

include(CPack)

