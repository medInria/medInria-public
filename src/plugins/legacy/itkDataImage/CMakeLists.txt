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
## Resolve dependencies
## #############################################################################

set(ITK_IMAGES_DEPENDENCIES
    ITKIOMeta
    #common formats for common images
    ITKIOBMP ITKIOJPEG ITKIOJPEG2000 ITKIOPNG ITKIOTIFF
    #Medical images formats
    ITKIOGIPL ITKIONRRD ITKIONIFTI ITKIOGIPL ITKIOPhilipsREC ITKIOGDCM
)

find_package(ITK REQUIRED COMPONENTS
      ITKIOVTK ITKVtkGlue ${ITK_IMAGES_DEPENDENCIES} ITKStatistics)

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

endif()

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} MODULE
  "${${TARGET_NAME}_QRC}"
  )
## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${${PROJECT_NAME}_VERSION})
string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
target_compile_definitions(${TARGET_NAME} PUBLIC ${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

target_compile_definitions(${TARGET_NAME} PUBLIC
    $<$<BOOL:${ITK_USE_SYSTEM_GDCM}>:ITK_USE_SYSTEM_GDCM>
    )
target_compile_definitions(${TARGET_NAME} PUBLIC
    $<$<BOOL:${USE_OSPRay}>:MED_USE_OSPRAY_4_VR_BY_CPU>
    )
#Prevent compiler warnings about valid std::copy.
target_compile_definitions(${TARGET_NAME} PUBLIC
    $<$<BOOL:${MSVC}>:_SCL_SECURE_NO_WARNINGS>
    )


## #############################################################################
## source
## #############################################################################
target_sources(${TARGET_NAME}
    PRIVATE ${${TARGET_NAME}_SOURCES}
    PUBLIC ${${TARGET_NAME}_TEMPLATES}
    PUBLIC ${${TARGET_NAME}_HEADERS}
)

## #############################################################################
## include directories.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PRIVATE ${${TARGET_NAME}_INCLUDE_DIRS}
  PRIVATE ${dtk_INCLUDE_DIRS}
  PRIVATE ${ITK_INCLUDE_DIRS}
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ${ITK_LIBRARIES}
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
