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
  RPI
  )

if (USE_DTKIMAGING)
  list(APPEND ${ep}_dependencies
       dtkImaging
       )
endif()

if (USE_Python)
  list(APPEND ${ep}_dependencies
      pyncpp
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
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}
  -DUSE_DTKIMAGING:BOOL=${USE_DTKIMAGING}
  -DUSE_OSPRay:BOOL=${USE_OSPRay}
  -DmedInria_VERSION:STRING=${${PROJECT_NAME}_VERSION}
  -DBUILD_ALL_PLUGINS=OFF
  -DBUILD_COMPOSITEDATASET_PLUGIN=OFF
  -DBUILD_EXAMPLE_PLUGINS=OFF
  )

set(cmake_cache_args
  -DDCMTK_ROOT:PATH=${DCMTK_ROOT}
  -Ddtk_ROOT:PATH=${dtk_ROOT}
  -DITK_ROOT:PATH=${ITK_ROOT}
  -DRPI_ROOT:PATH=${RPI_ROOT}
  -DTTK_ROOT:PATH=${TTK_ROOT}
  -DVTK_ROOT:PATH=${VTK_ROOT}
  -DQt${QT_VERSION_MAJOR}_DIR:PATH=${Qt${QT_VERSION_MAJOR}_DIR}
  -DCMAKE_BUILD_PARALLEL_LEVEL:STRING=8
  )

if(${SDK_GENERATION})
  list(APPEND cmake_cache_args -DCMAKE_INSTALL_PREFIX:PATH=${SDK_DIR})
else()
  list(APPEND cmake_cache_args -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)
endif()

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

if (USE_Python)
  list(APPEND cmake_cache_args
      -Dpyncpp_ROOT:PATH=${pyncpp_ROOT}
      )
endif()
  
## #############################################################################
## Add external-project
## #############################################################################

ExternalProject_Add(${ep}
  SOURCE_DIR ${medInria_SOURCE_DIR}
  BINARY_DIR ${medInria_BINARY_DIR}
  STAMP_DIR ${medinria_Stamp_DIR}
  INSTALL_DIR ${MEDINRIA_INSTALL_PREFIX}
  
  UPDATE_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  CMAKE_ARGS ${cmake_args}
  CMAKE_CACHE_ARGS ${cmake_cache_args}
  DEPENDS ${${ep}_dependencies}
  BUILD_ALWAYS 1
  ${EP_INSTAL_COMMAND}
  )

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_ROOT ${binary_dir} PARENT_SCOPE)
set(${ep}_DIR ${binary_dir}  PARENT_SCOPE)

ExternalProject_Get_Property(${ep} source_dir)
set(${ep}_SOURCE_DIR ${source_dir} PARENT_SCOPE)
  
if (WIN32)
  file(TO_NATIVE_PATH ${ITK_ROOT}                ITK_BIN_BASE)
  file(TO_NATIVE_PATH ${VTK_ROOT}                VTK_BIN_BASE)
  file(TO_NATIVE_PATH ${TTK_ROOT}                TTK_BIN_BASE)
  file(TO_NATIVE_PATH ${dtk_ROOT}                DTK_BIN_BASE)
  file(TO_NATIVE_PATH ${Qt${QT_VERSION_MAJOR}_DIR}/../../.. QTX_BIN_BASE)
  file(TO_NATIVE_PATH ${medInria_BINARY_DIR}     MED_BIN_BASE)
  
  file(TO_NATIVE_PATH "${QTX_BIN_BASE}/../../Tools" QT_TOOLS_DIR )

  set(CONFIG_MODE $<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>$<$<CONFIG:MinSizeRel>:MinSizeRel>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>)
  
  set(MED_BIN_BASE ${MED_BIN_BASE}\\bin)  
  
  add_custom_command(TARGET ${ep}
        POST_BUILD
        COMMAND for %%I in ( ${ITK_BIN_BASE}\\bin\\${CONFIG_MODE}\\*.dll ) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${VTK_BIN_BASE}\\bin\\${CONFIG_MODE}\\*.dll ) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${DTK_BIN_BASE}\\bin\\${CONFIG_MODE}\\*.dll ) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${TTK_BIN_BASE}\\bin\\*.dll                 ) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${QTX_BIN_BASE}\\bin\\*.dll                 ) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) 
        
        COMMAND if NOT EXIST ${MED_BIN_BASE}\\iconengines\\  (mkdir ${MED_BIN_BASE}\\iconengines\\)
        COMMAND if NOT EXIST ${MED_BIN_BASE}\\sqldrivers\\   (mkdir ${MED_BIN_BASE}\\sqldrivers\\)
        COMMAND if NOT EXIST ${MED_BIN_BASE}\\imageformats\\ (mkdir ${MED_BIN_BASE}\\imageformats\\)
        COMMAND if NOT EXIST ${MED_BIN_BASE}\\platforms\\    (mkdir ${MED_BIN_BASE}\\platforms\\)
        
        COMMAND for %%I in ( ${QTX_BIN_BASE}\\plugins\\iconengines\\*.dll  ) do (if EXIST ${MED_BIN_BASE}\\iconengines\\%%~nxI  (del /S ${MED_BIN_BASE}\\iconengines\\%%~nxI  & mklink /H ${MED_BIN_BASE}\\iconengines\\%%~nxI %%~fI)  else mklink /H ${MED_BIN_BASE}\\iconengines\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${QTX_BIN_BASE}\\plugins\\sqldrivers\\*.dll   ) do (if EXIST ${MED_BIN_BASE}\\sqldrivers\\%%~nxI   (del /S ${MED_BIN_BASE}\\sqldrivers\\%%~nxI   & mklink /H ${MED_BIN_BASE}\\sqldrivers\\%%~nxI %%~fI)   else mklink /H ${MED_BIN_BASE}\\sqldrivers\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${QTX_BIN_BASE}\\plugins\\imageformats\\*.dll ) do (if EXIST ${MED_BIN_BASE}\\imageformats\\%%~nxI (del /S ${MED_BIN_BASE}\\imageformats\\%%~nxI & mklink /H ${MED_BIN_BASE}\\imageformats\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\imageformats\\%%~nxI %%~fI) 
        COMMAND for %%I in ( ${QTX_BIN_BASE}\\plugins\\platforms\\*.dll    ) do (if EXIST ${MED_BIN_BASE}\\platforms\\%%~nxI    (del /S ${MED_BIN_BASE}\\platforms\\%%~nxI    & mklink /H ${MED_BIN_BASE}\\platforms\\%%~nxI %%~fI)    else mklink /H ${MED_BIN_BASE}\\platforms\\%%~nxI %%~fI)
		
        COMMAND for %%I in ( ${QT_TOOLS_DIR}\\OpenSSLv3\\Win_x64\\bin\\*.dll) do (if EXIST ${MED_BIN_BASE}\\%%~nxI (del /S ${MED_BIN_BASE}\\%%~nxI & mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI) else mklink /H ${MED_BIN_BASE}\\%%~nxI %%~fI)

    )
endif()



endif() #NOT USE_SYSTEM_ep

endfunction()
