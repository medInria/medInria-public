################################################################################
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
################################################################################

function(medInria_project)

set(ep medInria)

## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies 
  dtk 
  DCMTK 
  ITK 
  VTK 
  TTK 
  QtDCM
  RPI
  LogDemons
  )

if (USE_DTKIMAGING)
  list(APPEND ${ep}_dependencies
       dtkImaging
       )
endif()
  
## #############################################################################
## Prepare the project
## ############################################################################# 

EP_Initialisation(${ep}  
  USE_SYSTEM OFF 
  BUILD_SHARED_LIBS ON
  REQUIRED_FOR_PLUGINS ON
  )


if (NOT USE_SYSTEM_${ep})
## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
if (UNIX)
  set(${ep}_c_flags "${${ep}_c_flags} -Wall")
  set(${ep}_cxx_flags "${${ep}_cxx_flags} -Wall -Wno-unknown-pragmas")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(${ep}_cxx_flags "${${ep}_cxx_flags} -Wno-inconsistent-missing-override")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(${ep}_cxx_flags "${${ep}_cxx_flags} -fpermissive")
endif()

set(cmake_args
   ${ep_common_cache_args}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_medInria}
  -DCMAKE_C_FLAGS=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS=${${ep}_shared_linker_flags}  
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}
  -DUSE_DTKIMAGING:BOOL=${USE_DTKIMAGING}
  -DUSE_OSPRay:BOOL=${USE_OSPRay}
  -DmedInria_VERSION:STRING=${${PROJECT_NAME}_VERSION}
  -DBUILD_ALL_PLUGINS=OFF
  -DBUILD_COMPOSITEDATASET_PLUGIN=OFF
  -DBUILD_EXAMPLE_PLUGINS=OFF
   # External settings
  -DCMAKE_SUPERBUILD_SOURCE=${CMAKE_SUPERBUILD_SOURCE}
  -DSETTINGS_QRC_DIRECTORY=${SETTINGS_QRC_DIRECTORY}
  -DAPPLICATION_NAME=${APPLICATION_NAME}
  -DORGANIZATION_NAME=${ORGANIZATION_NAME}
  -DORGANIZATION_DOMAIN=${ORGANIZATION_DOMAIN}
  -DPROJECT_CONTACT=${PROJECT_CONTACT}
  -DPACKAGE_VENDOR=${PACKAGE_VENDOR}
  -DDOCUMENTATION_URL="${DOCUMENTATION_URL}"
  -DPROJECT_DESCRIPTION=${PROJECT_DESCRIPTION}
  -DLARGE_LOGO_DARK_THEME=${LARGE_LOGO_DARK_THEME}
  -DLARGE_LOGO_LIGHT_THEME=${LARGE_LOGO_LIGHT_THEME}
  -DWINDOW_ICON=${WINDOW_ICON}
  -DWINDOW_ICON_PATH=${WINDOW_ICON_PATH}
  -DMACOS_ICON_PATH=${MACOS_ICON_PATH}
  -DABOUT_FILE=${ABOUT_FILE}
  -DRELEASE_NOTES=${RELEASE_NOTES}
  -DADDITIONAL_AUTHORS_LIST="${ADDITIONAL_AUTHORS_LIST}"
  -DDESCRIPTION_HOMEPAGE=${DESCRIPTION_HOMEPAGE}
  -DDESCRIPTION_PACKAGING=${DESCRIPTION_PACKAGING}
  -DLICENSE_FILE=${LICENSE_FILE}
  -DLICENSE_EXTERNAL_FILE=${LICENSE_EXTERNAL_FILE}
  -DLICENSE_FILE_PATH=${LICENSE_FILE_PATH}
  -DREADME_FILE=${README_FILE}
  -DWELCOME_FILE=${WELCOME_FILE}
  -DEXPIRATION_TIME=${EXPIRATION_TIME}
  )

set(cmake_cache_args
  -DDCMTK_DIR:PATH=${DCMTK_DIR}
  -Ddtk_DIR:PATH=${dtk_DIR}
  -DITK_DIR:PATH=${ITK_DIR}
  -DQtDCM_DIR:PATH=${QtDCM_DIR}
  -DRPI_DIR:PATH=${RPI_DIR}
  -DTTK_DIR:PATH=${TTK_DIR}
  -DVTK_DIR:PATH=${VTK_DIR}
  -DQt5_DIR:PATH=${Qt5_DIR}
  -DLogDemons_DIR:PATH=${LogDemons_DIR}
  -DBoost_INCLUDE_DIR:PATH=${Boost_INCLUDE_DIR}
  )

if (${USE_FFmpeg})
  list(APPEND cmake_args
  -DUSE_FFmpeg=${USE_FFmpeg})
endif() 

if (USE_DTKIMAGING)
  set(cmake_args
    ${cmake_args}
    -DdtkImaging_DIR:PATH=${dtkImaging_DIR}
    )
endif()
  
## #############################################################################
## Add external-project
## #############################################################################

ExternalProject_Add(${ep}
  SOURCE_DIR ${medInria_SOURCE_DIR}
  BINARY_DIR ${medInria_BINARY_DIR}
  STAMP_DIR ${medinria_Stamp_DIR}
  UPDATE_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  CMAKE_ARGS ${cmake_args}
  CMAKE_CACHE_ARGS ${cmake_cache_args}
  DEPENDS ${${ep}_dependencies}
  INSTALL_COMMAND ""
  BUILD_ALWAYS 1
  )

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

ExternalProject_Get_Property(${ep} source_dir)
set(${ep}_SOURCE_DIR ${source_dir} PARENT_SCOPE)
  
  
if (WIN32)
  file(TO_NATIVE_PATH ${ITK_DIR}                 ITK_BIN_BASE)
  file(TO_NATIVE_PATH ${VTK_DIR}                 VTK_BIN_BASE)
  file(TO_NATIVE_PATH ${dtk_DIR}                 DTK_BIN_BASE)
  file(TO_NATIVE_PATH ${QtDCM_DIR}               DCM_BIN_BASE)
  file(TO_NATIVE_PATH ${_qt5Core_install_prefix} QT5_BIN_BASE)
  file(TO_NATIVE_PATH ${medInria_BINARY_DIR}     MED_BIN_BASE)
  
  set(CONFIG_MODE $<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>$<$<CONFIG:MinSizeRel>:MinSizeRel>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>)
  
  set(MED_BIN_BASE ${MED_BIN_BASE}\\${CONFIG_MODE}\\bin)  
  
  add_custom_command(TARGET ${ep}
        POST_BUILD
        COMMAND for %%I in ( ${ITK_BIN_BASE}\\bin\\${CONFIG_MODE}\\*.dll ) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${VTK_BIN_BASE}\\bin\\${CONFIG_MODE}\\*.dll ) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${DTK_BIN_BASE}\\bin\\${CONFIG_MODE}\\*.dll ) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${DCM_BIN_BASE}\\bin\\${CONFIG_MODE}\\*.dll ) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${QT5_BIN_BASE}\\bin\\*.dll                 ) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) 
    )
endif()



endif() #NOT USE_SYSTEM_ep

endfunction()
