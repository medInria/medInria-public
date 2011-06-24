### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Sat Oct  3 18:22:21 2009 (+0200)
## Version: $Id$
## Last-Updated: Wed May  4 10:55:34 2011 (+0200)
##           By: Julien Wintz
##     Update #: 43
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(medCore)

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  med4DAbstractViewInteractor.h
  medAbstractData.h  
  medAbstractDataSource.h
  medAbstractDataSourceFactory.h
  medAbstractDbController.h
  medAbstractView.h
  medAbstractViewFiberInteractor.h
  medAttachedData.h
  medDataIndex.h
  medDataManager.h
  medDbControllerFactory.h
  medJobItem.h
  medJobManager.h
  medMeshAbstractViewInteractor.h
  medMessageController.h
  medMetaDataHelper.h
  medPluginManager.h
  medProgressFilter.h
  medRunnableProcess.h
  medSettingsManager.h
  medStorage.h
  medViewManager.h
)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC}
  medAbstractDataCollection.h
  medCoreExport.h
)


set(${PROJECT_NAME}_SOURCES
  medAbstractData.cpp
  medAbstractDataCollection.cpp
  medAbstractDataSource.cpp
  medAbstractDataSourceFactory.cpp
  medAbstractDbController.cpp
  medAbstractView.cpp
  medAbstractViewFiberInteractor.cpp
  medAttachedData.cpp
  medDataIndex.cpp
  medDataManager.cpp
  medDbControllerFactory.cpp
  medJobItem.cpp
  medJobManager.cpp  
  medMessageController.cpp
  medMetaDataHelper.cpp
  medPluginManager.cpp
  medProgressFilter.cpp
  medRunnableProcess.cpp
  medSettingsManager.cpp
  medStorage.cpp
  medViewManager.cpp
)

## #################################################################
## Wrapping
## #################################################################

if(SWIG_FOUND)
    set(${PROJECT_NAME}_WRAP_DEPENDS ${${PROJECT_NAME}_HEADERS})
    if(PYTHONLIBS_FOUND)
        dtk_wrap(${PROJECT_NAME} ${PROJECT_NAME}_SOURCES_WRAP medcore python ${CMAKE_CURRENT_SOURCE_DIR}/medCore.i ${${PROJECT_NAME}_WRAP_DEPENDS})
    endif(PYTHONLIBS_FOUND)
    if(TCL_FOUND)
        dtk_wrap(${PROJECT_NAME} ${PROJECT_NAME}_SOURCES_WRAP medcore tcl    ${CMAKE_CURRENT_SOURCE_DIR}/medCore.i ${${PROJECT_NAME}_WRAP_DEPENDS})
    endif(TCL_FOUND)
endif(SWIG_FOUND)
## #################################################################
## Build rules
## #################################################################

if(NOT MSVC)
add_definitions(-Wno-write-strings)
add_definitions(-Wformat=0)
endif(NOT MSVC)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_SHARED)
add_definitions(-DQT_NO_DEBUG)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES_WRAP})

target_link_libraries(${PROJECT_NAME} 
    ${QT_LIBRARIES}
    dtkCore)
if(TCL_FOUND)
  target_link_libraries(${PROJECT_NAME} ${TCL_LIBRARY})
endif(TCL_FOUND)

if(PYTHONLIBS_FOUND)
  target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})
endif(PYTHONLIBS_FOUND)

IF(WIN32)		
  target_link_libraries(${PROJECT_NAME} psapi.lib)
ENDIF(WIN32)

# Include header files for Visual Studio.
SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Handling of generated script modules
## #################################################################
if (SWIG_FOUND)
    if (PYTHONLIBS_FOUND)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
          DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/medCore.i
          COMMAND ${CMAKE_COMMAND} ARGS -E  make_directory ${CMAKE_BINARY_DIR}/modules
          COMMAND ${CMAKE_COMMAND} ARGS -E  copy ${${PROJECT_NAME}_BINARY_DIR}/medcore.py ${CMAKE_BINARY_DIR}/modules
          COMMENT "-- Moving python modules to ${CMAKE_BINARY_DIR}/modules"
        )
    set(${PROJECT_NAME}_MODULES ${CMAKE_BINARY_DIR}/modules/medcore.py)
    endif(PYTHONLIBS_FOUND)
endif(SWIG_FOUND)


## #################################################################
## Installation
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(FILES ${${PROJECT_NAME}_MODULES} DESTINATION modules)
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
