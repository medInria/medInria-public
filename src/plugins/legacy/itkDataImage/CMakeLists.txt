################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

set(TARGET_NAME itkDataImagePlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${${PROJECT_NAME}_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")


## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

if (ITK_USE_SYSTEM_GDCM)
  add_definitions(-DITK_USE_SYSTEM_GDCM)
endif()

if (USE_OSPRay)
  add_definitions(-DMED_USE_OSPRAY_4_VR_BY_CPU)
endif()

find_package(DCMTK REQUIRED NO_MODULE)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  . # current dir
  datas
  interactors
  limit
  manager
  navigators
  readers
  writers
  )
list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

## #############################################################################
## add library
## #############################################################################

# Due to the large number of template functions, windows needs an extra compile flag.
# Currently this only affects x64/debug builds, but enabling this flag for others does no harm.
if (MSVC)
    set(bigobjFile itkDataImage.cpp)
    get_source_file_property(bigobjFile_COMPILE_FLAGS ${bigobjFile} COMPILE_FLAGS)
    if(${bigobjFile_COMPILE_FLAGS} STREQUAL NOTFOUND)
        set_source_files_properties(${bigobjFile}
          PROPERTIES COMPILE_FLAGS
          "/bigobj"
          )
    else()
        set_source_files_properties(${bigobjFile}
          PROPERTIES COMPILE_FLAGS
         "${bigobjFile_COMPILE_FLAGS} /bigobj"
         )
    endif()

  #Prevent compiler warnings about valid std::copy.
  if(MSVC)
      add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  endif()

endif()

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  )


## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PRIVATE ${${TARGET_NAME}_INCLUDE_DIRS}
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  Qt5::Core
  Qt5::Widgets
  dtkCore
  dtkLog
  ITKIOImageBase
  ITKIOBMP
  ITKIOBruker
  ITKIOLSM
  ITKIOJPEG
  ITKIOPNG
  ITKIOVTK
  ITKIOGDCM
  ITKIOGE
  ITKIOMINC
  ITKNrrdIO
  ITKIONRRD
  ITKIOMeta
  ITKIONIFTI
  ITKIOGIPL
  ITKIOHDF5
  ITKIOBioRad
  ITKIOStimulate
  ITKIOMRC
  ITKStatistics
  ${ITKReview_LIBRARIES} #TODO Get rid of this ASAP
  medCore
  medImageIO
  medLog
  medVtkInria
  )

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})


## #############################################################################
## Build tests
## #############################################################################

if(${PROJECT_NAME}_BUILD_TESTS)
  add_subdirectory(tests)
endif()
