
project(dcmtkTool)

cmake_minimum_required(VERSION 2.8.1)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})


INCLUDE(files.cmake)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})


# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  "./medDcmtk"
  "./medLogger"
)


# Set your files and resources here  
SET(SimpleViewSrcs main.cpp SimpleView.cpp)
SET(SimpleViewUI SimpleView.ui)
SET(SimpleViewHeaders SimpleView.h)
SET(SimpleViewResources Icons/icons.qrc)

# The rest should just work (sure...)
QT4_WRAP_UI(UISrcs ${SimpleViewUI})
QT4_WRAP_CPP(MOCSrcs ${SimpleViewHeaders} ${OTHER_MOC_FILES})
QT4_ADD_RESOURCES(ResourceSrcs ${SimpleViewResources})

SOURCE_GROUP("Resources" FILES
  ${SimpleViewUI}
  ${SimpleViewResources}
  ${EXE_ICON}
)

SOURCE_GROUP("Generated" FILES
  ${UISrcs}
  ${MOCSrcs}
  ${ResourceSrcs}
  ${RCS_SOURCES}
)

ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB -DQT3_SUPPORT)
SET_SOURCE_FILES_PROPERTIES(${SimpleViewSrcs} PROPERTIES
                            OBJECT_DEPENDS "${UISrcs}")

# It's nice to have the ui in the windows project file...just double click on it
# and designer comes up on that ui file :)
IF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
  SET (SimpleViewSrcs ${SimpleViewSrcs} ${SimpleViewUI})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
IF (${CMAKE_BUILD_TOOL} MATCHES "devenv")
  SET (SimpleViewSrcs ${SimpleViewSrcs} ${SimpleViewUI})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "devenv")


find_package(DCMTK2 REQUIRED)
include_directories(${DCMTK_INCLUDE_DIR})

add_executable(${PROJECT_NAME} MACOSX_BUNDLE 
		${SimpleViewSrcs} 
		${UISrcs} 
		${MOCSrcs} 
		${ResourceSrcs} 
		${MED_DCMTK_SOURCES}
		${MED_LOGGER_SOURCES}
		${MED_GENERAL_SOURCES}
		)
IF(WIN32)		
	SET(WIN32_STD_LIBRARIES 
			ws2_32
			netapi32
			wsock32
			)
ENDIF(WIN32)
		
#Build Groups for Visual Studio
SOURCE_GROUP( "DCMTK" FILES ${MED_DCMTK_SOURCES})
SOURCE_GROUP( "LOGGER" FILES ${MED_LOGGER_SOURCES})
		
target_link_libraries(${PROJECT_NAME}
	${DCMTK_LIBRARIES}
	${QT_LIBRARIES}
	${WIN32_STD_LIBRARIES} 		
	)
	
# Disable deprecation warnings for standard C and STL functions in VS2005
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  ADD_DEFINITIONS(-D_SCL_SECURE_NO_DEPRECATE)
  #SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:console")