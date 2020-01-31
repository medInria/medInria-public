function(qwt_project)

set(ep qwt)

## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies "")
  
## #############################################################################
## Prepare the project
## ############################################################################# 

EP_Initialisation(${ep}
  USE_SYSTEM OFF 
  BUILD_SHARED_LIBS ON
  REQUIRED_FOR_PLUGINS OFF
)

if (NOT USE_SYSTEM_${ep})

## #############################################################################
## Define repository where get the sources
## #############################################################################

set(git_url ${GITHUB_PREFIX}osakared/qwt.git)
set(git_tag trunk)

## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
# Compile using c++11 standard
set(${ep}_cxx_flags "${${ep}_cxx_flags} -Wall -std=c++11")
set(${ep}_c_flags "${${ep}_c_flags} -Wall")

find_package(Qt5 REQUIRED Core)

set (MAKE_PROGRAM ${CMAKE_MAKE_PROGRAM})

get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)

set(QWT_INSTALL_PREFIX ${EP_PATH_SOURCE}/../build/qwt)

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_C_FLAGS:STRING=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS:STRING=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS:STRING=${${ep}_shared_linker_flags}
  -DCMAKE_INSTALL_PREFIX:PATH=${EP_PATH_SOURCE}/../build/qwt
  -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
)

ep_GeneratePatchCommand(${ep} QWT_PATCH_COMMAND qwt-6.3.patch)

## #############################################################################
## Add external-project
## #############################################################################
epComputPath(${ep})

ExternalProject_Add(${ep}
  PREFIX ${EP_PATH_SOURCE}
  SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
  BINARY_DIR ${build_path}
  TMP_DIR ${tmp_path}
  STAMP_DIR ${stamp_path}
  GIT_REPOSITORY ${git_url}
  GIT_TAG ${git_tag}
  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  CMAKE_ARGS ${cmake_args}
  DEPENDS ${${ep}_dependencies}
  PATCH_COMMAND ${QWT_PATCH_COMMAND}
  CONFIGURE_COMMAND ${QT_QMAKE_EXECUTABLE} ${SPEC} <SOURCE_DIR>/qwt/qwt.pro
  BUILD_COMMAND ${MAKE_PROGRAM} sub-src
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
)

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
