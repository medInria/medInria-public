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

set(TARGET_NAME itkFiltersPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(ITK)
include(${ITK_USE_FILE})


## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )


## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  )


## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${TARGET_NAME}_PCH
    ${MEDPLUGINSPCH_H}
    ${MEDPLUGINSPCH_CPP}
    ${${TARGET_NAME}_SOURCES}
    )
endif()


## #############################################################################
## add library
## #############################################################################

# Due to the large number of template functions, windows needs an extra compile flag.
# Currently this only affects x64/debug builds, but enabling this flag for others does no harm.
if (MSVC)
    set(bigobjFile itkFilters.cpp)
    get_source_file_property(bigobjFile_COMPILE_FLAGS ${bigobjFile} COMPILE_FLAGS)
    if (${bigobjFile_COMPILE_FLAGS} STREQUAL NOTFOUND)
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

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_PCH}
  ${${TARGET_NAME}_CFILES}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ${QT_LIBRARIES}
  dtkCore
  dtkLog
  ITKCommon
  medCore
  )


## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})

