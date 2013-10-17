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

project(medSql)

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_DEVEL_HEADERS_MOC
  medDatabaseController.h
  medDatabaseControllerImpl.h
  medDatabaseImporter.h
)

set(${PROJECT_NAME}_DEVEL_HEADERS
  medSqlExport.h
)

set(${PROJECT_NAME}_HEADERS_MOC
  ${${PROJECT_NAME}_DEVEL_HEADERS_MOC}
  medAbstractDatabaseImporter.h
  medDatabaseExporter.h
  medDatabaseModel.h  
  medDatabaseNonPersistentController.h
  medDatabaseNonPersistentControllerImpl.h
  medDatabaseNonPersistentItem.h
  medDatabaseNonPersistentImporter.h
  medDatabaseProxyModel.h
  medDatabaseReader.h
  medDatabaseRemover.h
  )

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_DEVEL_HEADERS}
  ${${PROJECT_NAME}_HEADERS_MOC}
  medDatabaseItem.h
  )

set(${PROJECT_NAME}_SOURCES
  medAbstractDatabaseImporter.cpp
  medDatabaseController.cpp
  medDatabaseControllerImpl.cpp
  medDatabaseExporter.cpp
  medDatabaseImporter.cpp
  medDatabaseItem.cpp
  medDatabaseModel.cpp  
  medDatabaseNonPersistentController.cpp
  medDatabaseNonPersistentControllerImpl.cpp
  medDatabaseNonPersistentItem.cpp
  medDatabaseNonPersistentImporter.cpp
  medDatabaseProxyModel.cpp
  medDatabaseReader.cpp
  medDatabaseRemover.cpp
  )

## #################################################################
## Build rules
## #################################################################

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_SHARED)
add_definitions(-DQT_NO_DEBUG)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

if (MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH 
    "medPch.h" "../medPch/medPch.cpp" 
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_SOURCES_MOC}
    ${${PROJECT_NAME}_SOURCES_WRAP})
endif()


add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_WRAP})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore
  dtkLog
  medCore
  )

# Include header files for Visual Studio.
source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})


if (BUILD_TESTING)
  add_subdirectory (Testing)
endif()


## #################################################################
## Installation
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
