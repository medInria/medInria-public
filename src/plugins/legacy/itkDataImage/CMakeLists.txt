################################################################################
#
# medInria

# Copyright (c) INRIA 2013. All rights reserved.

# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
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

set(ITK_IMAGES_DEPENDENCIES
    ITKIOMeta

    #common formats for common images
    ITKIOBMP
    ITKIOJPEG
    ITKIOJPEG2000
    ITKIOPNG
    ITKIOTIFF

    #Medical images formats
    ITKIOBruker
    ITKIOHDF5
    ITKIOGIPL
    ITKIONIFTI
    ITKIONRRD
    ITKIOPhilipsREC
)

find_package(ITK REQUIRED COMPONENTS ITKIOImageBase ITKGDCM ITKIOGDCM ITKIOVTK ITKVtkGlue ${ITK_IMAGES_DEPENDENCIES})
include(${ITK_USE_FILE})

# itkLogForwarder used through medLog
# vtkImageView2D  used through medVtkInria

#WARNING USE VTK through ITKIOVTK and medLog, medVtkInria

if (ITK_USE_SYSTEM_GDCM)
  add_definitions(-DITK_USE_SYSTEM_GDCM)
endif()

if (USE_OSPRay)
  add_definitions(-DMED_USE_OSPRAY_4_VR_BY_CPU)
endif()

find_package(DCMTK REQUIRED NO_MODULE)


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
  gdcmCommon
  ${ITK_LIBRARIES}
  medCore
  medImageIO
  medLog
  medVtkInria
  medUtilities
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
