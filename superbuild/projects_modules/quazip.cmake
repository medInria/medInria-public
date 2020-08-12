function(quazip_project)

set(ep quazip)

## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies "")
  
## #############################################################################
## Prepare the project
## ############################################################################# 

EP_Initialisation(${ep}
  USE_SYSTEM OFF 
  BUILD_SHARED_LIBS OFF
  REQUIRED_FOR_PLUGINS ON
)

if (NOT USE_SYSTEM_${ep})

## #############################################################################
## Define repository where get the sources
## #############################################################################

set(git_url ${GITHUB_PREFIX}stachenov/quazip.git)
set(git_tag master)

## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
# Compile using c++11 standard
set(${ep}_cxx_flags "${${ep}_cxx_flags} -Wall -std=c++11")
set(${ep}_c_flags "${${ep}_c_flags} -Wall")

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_externals_projects}
  -DCMAKE_C_FLAGS:STRING=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS:STRING=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS:STRING=${${ep}_shared_linker_flags}
  -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
)

## #############################################################################
## Add external-project
## #############################################################################
if(APPLE)
  set(SPEC -spec macx-clang)
endif()

set(MAKE_PROGRAM ${CMAKE_MAKE_PROGRAM})
if(WIN32)
  set(MAKE_PROGRAM nmake)
endif()

find_package(Qt5 REQUIRED Core)
get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)

find_package(ZLIB REQUIRED)

## quazip fails to compile if zlib is found in an already
## included default path.
if (UNIX)
  string(COMPARE EQUAL ${ZLIB_INCLUDE_DIRS} "/usr/include" _cmp)
  if (_cmp)
    set(ZLIB_INCLUDE_DIRS "")
  endif (_cmp)
endif (UNIX)

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
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ${QT_QMAKE_EXECUTABLE} ${SPEC} PREFIX=${build_path} CONFIG+=staticlib LIBS+=${ZLIB_LIBRARIES} INCLUDEPATH+=${ZLI_INCLUDE_DIRS} <SOURCE_DIR>/quazip.pro
  BUILD_COMMAND ${MAKE_PROGRAM} sub-quazip-install_subtargets
  INSTALL_COMMAND ""
)

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${build_path} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
