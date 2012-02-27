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
  medDataManager.h
  medMessageController.h
  medMetaDataKeys.h
  medViewManager.h
  medPluginManager.h
  medAbstractView.h
  medAbstractViewCollection.h
  med4DAbstractViewInteractor.h
  medMeshAbstractViewInteractor.h
  medJobItem.h
  medJobManager.h
  medSettingsManager.h
  medDbControllerFactory.h
  medAbstractDbController.h
  medAbstractDataSource.h
  medAbstractDataSourceFactory.h
  medStorage.h
  medRunnableProcess.h
  medAbstractViewFiberInteractor.h
  medAbstractData.h
  medAbstractDataCollection.h
  medAbstractDataImage.h
  medAbstractDataImage2D.h
  medAbstractDataImage3D.h
  medAbstractDataImage4D.h
  medAbstractDataMesh.h
  medAttachedData.h
  medAnnotationData.h
  medAbstractAnnotationViewInteractor.h
  medViewEventFilter.h
  annotationData/medImageMaskAnnotationData.h
  annotationData/medSeedPointAnnotationData.h
)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC}
  medCoreExport.h
  medDataIndex.h
  medDataReaderWriter.h
  medAbstractDataTypedImage.h
  medAbstractDataTypedImage2D.h
  medAbstractDataTypedImage3D.h
  medAbstractDataTypedImage4D.h
  medAbstractDatabaseItem.h
  medAbstractViewCoordinates.h
)


set(${PROJECT_NAME}_SOURCES
  medDataIndex.cpp
  medDataManager.cpp
  medDataReaderWriter.cpp
  medMessageController.cpp
  medMetaDataKeys.cpp
  medViewManager.cpp
  medPluginManager.cpp
  medAbstractView.cpp
  medAbstractViewCoordinates.cpp
  medAbstractViewCollection.cpp
  medStorage.cpp
  medJobItem.cpp
  medJobManager.cpp
  medSettingsManager.cpp
  medDbControllerFactory.cpp
  medAbstractDbController.cpp
  medRunnableProcess.cpp
  medAbstractDataSource.cpp
  medAbstractDataSourceFactory.cpp
  medAbstractViewFiberInteractor.cpp
  medAbstractData.cpp
  medAbstractDataCollection.cpp
  medAbstractDataImage.cpp
  medAbstractDataImage2D.cpp
  medAbstractDataImage3D.cpp
  medAbstractDataImage4D.cpp
  medAbstractDataMesh.cpp
  medAttachedData.cpp
  medAbstractDatabaseItem.cpp
  medAnnotationData.cpp
  medAbstractAnnotationViewInteractor.cpp
  medViewEventFilter.cpp
  annotationData/medImageMaskAnnotationData.cpp
  annotationData/medSeedPointAnnotationData.cpp
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

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH
    "medPch.h" "../medPch/medPch.cpp"
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_SOURCES_MOC}
    ${${PROJECT_NAME}_SOURCES_WRAP})
endif(MEDINRIA_USE_PRECOMPILED_HEADERS)

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_SOURCES_PCH}
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

# Include header files for Visual Studio.
SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Installation
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(FILES ${${PROJECT_NAME}_MODULES} DESTINATION modules)
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
