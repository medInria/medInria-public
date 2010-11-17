
project(dcmtkSpeedTest)

cmake_minimum_required(VERSION 2.8.1)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)


## #################################################################
## Resolve dependencies
## #################################################################

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(DCMTK2 REQUIRED)
include_directories(${DCMTK_INCLUDE_DIR})

IF(WIN32)		
	SET(WIN32_STD_LIBRARIES 
		ws2_32
		netapi32
		wsock32)
ENDIF(WIN32)

## #################################################################
## Input
## #################################################################

include(files.cmake)

## #################################################################
## Build rules
## #################################################################
	
add_definitions(-DHAVE_CONFIG_H)

qt4_wrap_cpp(WRAP ${OTHER_MOC_FILES})

include_directories(
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  "../../medDcmtk"
  "../../medLogger"
)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE 
		${WRAP}
		${MED_DCMTK_SOURCES}
		${MED_LOGGER_SOURCES}
		${MED_GENERAL_SOURCES}
		)

		
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