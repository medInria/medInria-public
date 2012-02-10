# - find DCMTK libraries
#
	
#  DCMTK_INCLUDE_DIR   - Directories to include to use DCMTK
#  DCMTK_LIBRARIES     - Files to link against to use DCMTK
#  DCMTK_FOUND         - If false, don't try to use DCMTK
#  DCMTK_DIR           - (optional) Build or install directory for DCMTK
#  DCMTK_SOURCE_DIR    - (optional) Source directory for DCMTK
#
# DCMTK_DIR can be used to make it simpler to find the various include
# directories and compiled libraries if you've just compiled it in the
# source tree. Just set it to the root of the tree where you extracted
# the source.
#
# Written for VXL by Amitha Perera.
# 

unset(DCMTK_FOUND)

# Construct consitent error messages for use below.
set(DCMTK_DIR_DESCRIPTION "directory containing DCMTK install files.  This is either the root of the build or install tree.")
set(DCMTK_SOURCE_DIR_DESCRIPTION "directory containing DCMTK source files.  This is root of the source tree.")
set(DCMTK_DIR_MESSAGE "DCMTK not found.  Set the DCMTK_DIR cmake cache entry to the ${DCMTK_DIR_DESCRIPTION}")
set(DCMTK_SOURCE_DIR_MESSAGE "DCMTK not found.  Set the DCMTK_SOURCE_DIR cmake cache entry to the ${DCMTK_SOURCE_DIR_DESCRIPTION}")

set(DCMTK_DIR $ENV{DCMTK_DIR})
set(DCMTK_SOURCE_DIR $ENV{DCMTK_SOURCE_DIR})

find_path( DCMTK_root_INCLUDE_DIR dcmtk/config/osconfig.h
  PATHS
  ${DCMTK_DIR}/include
  ${DCMTK_DIR}/config/include
)

# A list of (incName,incFile) pairs specifying the name to generate and the file to search for.
# DCMTK_${incName}_INCLUDE_DIR is set by finding ${incFile}
set(dcmtk_INCLUDE_DIR_NAMES_FILES
  config    osconfig.h
  ofstd     ofstdinc.h
  oflog     logger.h
  dcmdata   dctypes.h
  dcmimgle  dcmimage.h
  dcmjpeg   djdecode.h
  dcmjpls   djlsutil.h
  )

# A list of libraries to search for.
# DCMTK_${libName}_LIBRARY and optionally, DCMTK_${libName}_LIBRARY_DEBUG are set.
set(dcmtk_LIBRARY_NAMES
  ofstd oflog dcmdata dcmimgle dcmjpeg ijg8 ijg12 ijg16 dcmqrdb dcmnet dcmjpls charls
  )

#Loop over include dirs, find them.
list(LENGTH dcmtk_INCLUDE_DIR_NAMES_FILES dcmtk_INCLUDE_DIR_NAMES_FILES_LEN)

set(elemNum 0)
while(${elemNum} LESS ${dcmtk_INCLUDE_DIR_NAMES_FILES_LEN} )
  list(GET dcmtk_INCLUDE_DIR_NAMES_FILES ${elemNum} incName)
  math(EXPR elemNum "${elemNum} + 1")
  list(GET dcmtk_INCLUDE_DIR_NAMES_FILES ${elemNum} incFile)
  math(EXPR elemNum "${elemNum} + 1")

  find_path( DCMTK_${incName}_INCLUDE_DIR ${incFile}
    ${DCMTK_DIR}/${incName}/include
    ${DCMTK_DIR}/include/${incName}
    ${DCMTK_DIR}/include/dcmtk/${incName}
    ${DCMTK_DIR}/config/include/dcmtk/${incName}
  )

  if(NOT DCMTK_${incName}_INCLUDE_DIR) 
    find_path( DCMTK_${incName}_INCLUDE_DIR dcmtk/${incName}/${incFile}
      ${DCMTK_SOURCE_DIR}/${incName}/include
    )
  endif()

  if(NOT DCMTK_${incName}_INCLUDE_DIR) 
    message(WARNING "FindDCMTK2.cmake : DCMTK_${incName}_INCLUDE_DIR=${DCMTK_${incName}_INCLUDE_DIR}")
  endif()
endwhile()


get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)

set(LIBSUFFIX
)
if (DEFINED LIB64)
  if (${LIB64} STREQUAL "TRUE")
    set(LIBSUFFIX 64)
  endif()
endif()

# Loop over libraries, finding them.
foreach(libName ${dcmtk_LIBRARY_NAMES})

  find_library( DCMTK_${libName}_LIBRARY ${libName}
    ${DCMTK_DIR}/${libName}/libsrc
    ${DCMTK_DIR}/${libName}/libsrc/Release
    ${DCMTK_DIR}/${libName}/Release
    ${DCMTK_DIR}/${libName}/libsrc/RelWithDebInfo
    ${DCMTK_DIR}/${libName}/RelWithDebInfo
    ${DCMTK_DIR}/${libName}/libsrc/MinSizeRel
    ${DCMTK_DIR}/${libName}/MinSizeRel
    ${DCMTK_DIR}/dcmjpeg/lib${libName}
    ${DCMTK_DIR}/dcmjpeg/lib${libName}/Release
    ${DCMTK_DIR}/dcmjpeg/lib${libName}/RelWithDebInfo
    ${DCMTK_DIR}/dcmjpeg/lib${libName}/MinSizeRel
    ${DCMTK_DIR}/dcmjpls/lib${libName}
    ${DCMTK_DIR}/dcmjpls/lib${libName}/Release
    ${DCMTK_DIR}/dcmjpls/lib${libName}/RelWithDebInfo
    ${DCMTK_DIR}/dcmjpls/lib${libName}/MinSizeRel
    ${DCMTK_DIR}/lib${LIBSUFFIX}
    ${DCMTK_DIR}/lib${LIBSUFFIX}/dcmtk
    ${DCMTK_DIR}/lib${LIBSUFFIX}/Release
    ${DCMTK_DIR}/lib${LIBSUFFIX}/RelWithDebInfo
    ${DCMTK_DIR}/lib${LIBSUFFIX}/MinSizeRel
  )

 find_library( DCMTK_${libName}_LIBRARY_DEBUG ${libName}
    ${DCMTK_DIR}/${libName}/libsrc/Debug
    ${DCMTK_DIR}/${libName}/Debug
    ${DCMTK_DIR}/lib${LIBSUFFIX}/Debug
    ${DCMTK_DIR}/dcmjpeg/lib${libName}/Debug
    ${DCMTK_DIR}/dcmjpls/lib${libName}/Debug
  )

  if(NOT DCMTK_${libName}_LIBRARY) 
    message(WARNING "FindDCMTK2.cmake : DCMTK_${libName}_LIBRARY=${DCMTK_${libName}_LIBRARY}")
  endif()

endforeach()


if( DCMTK_root_INCLUDE_DIR
    AND DCMTK_config_INCLUDE_DIR 
    AND DCMTK_ofstd_INCLUDE_DIR 
    AND DCMTK_ofstd_LIBRARY
    AND DCMTK_dcmdata_INCLUDE_DIR
    AND DCMTK_dcmdata_LIBRARY
    AND DCMTK_dcmimgle_INCLUDE_DIR
    AND DCMTK_dcmimgle_LIBRARY 
    AND DCMTK_dcmjpeg_LIBRARY 
    AND DCMTK_dcmjpls_LIBRARY
    AND DCMTK_dcmjpls_INCLUDE_DIR)

  set( DCMTK_FOUND 1)

  set( DCMTK_INCLUDE_DIR
    ${DCMTK_root_INCLUDE_DIR}
    ${DCMTK_config_INCLUDE_DIR}
    ${DCMTK_ofstd_INCLUDE_DIR}
    ${DCMTK_dcmdata_INCLUDE_DIR}
    ${DCMTK_dcmimgle_INCLUDE_DIR}
    ${DCMTK_dcmjpeg_INCLUDE_DIR}
    ${DCMTK_dcmjpls_INCLUDE_DIR}
  )
  if( WIN32 )
    set (OPTIMIZED optimized)
  endif(WIN32)

  set( DCMTK_LIBRARIES
    ${OPTIMIZED} ${DCMTK_dcmjpeg_LIBRARY}
    ${OPTIMIZED} ${DCMTK_ijg8_LIBRARY}
    ${OPTIMIZED} ${DCMTK_ijg12_LIBRARY}
    ${OPTIMIZED} ${DCMTK_ijg16_LIBRARY}
    ${OPTIMIZED} ${DCMTK_dcmimgle_LIBRARY}
    ${OPTIMIZED} ${DCMTK_dcmdata_LIBRARY}
    ${OPTIMIZED} ${DCMTK_charls_LIBRARY}
    ${OPTIMIZED} ${DCMTK_dcmjpls_LIBRARY}
  )
  set (DCMTK_LIBRARIES_DEBUG
    debug     ${DCMTK_dcmjpeg_LIBRARY_DEBUG}
    debug     ${DCMTK_ijg8_LIBRARY_DEBUG}
    debug     ${DCMTK_ijg12_LIBRARY_DEBUG}
    debug     ${DCMTK_ijg16_LIBRARY_DEBUG}
    debug     ${DCMTK_dcmimgle_LIBRARY_DEBUG}
    debug     ${DCMTK_dcmdata_LIBRARY_DEBUG}
    debug     ${DCMTK_charls_LIBRARY_DEBUG}
    debug     ${DCMTK_dcmjpls_LIBRARY_DEBUG}
  )

  if ( DCMTK_oflog_LIBRARY )#oflog only in dcmtk>=3.5.5
    set( DCMTK_INCLUDE_DIR
      ${DCMTK_oflog_INCLUDE_DIR}
      ${DCMTK_INCLUDE_DIR}
    )
    set( DCMTK_LIBRARIES
      ${DCMTK_LIBRARIES}
      ${OPTIMIZED} ${DCMTK_oflog_LIBRARY} #must be defined before ofstd
    )
    set( DCMTK_LIBRARIES_DEBUG
      ${DCMTK_LIBRARIES_DEBUG}
      debug     ${DCMTK_oflog_LIBRARY_DEBUG} #must be defined before ofstd 
    )

  endif ( DCMTK_oflog_LIBRARY )
  
  set( DCMTK_LIBRARIES 
   ${DCMTK_LIBRARIES}
   ${OPTIMIZED} ${DCMTK_ofstd_LIBRARY}
  )
  set( DCMTK_LIBRARIES_DEBUG 
   ${DCMTK_LIBRARIES_DEBUG}
   debug     ${DCMTK_ofstd_LIBRARY_DEBUG}
  )


  if(NOT WIN32)
    set( DCMTK_LIBRARIES
    ${DCMTK_LIBRARIES}
    z
    )   
  endif(NOT WIN32)
  
if(DCMTK_dcmqrdb_LIBRARY)
  set( DCMTK_LIBRARIES
    ${OPTIMIZED} ${DCMTK_dcmqrdb_LIBRARY}
    ${DCMTK_LIBRARIES}
    )
  set( DCMTK_LIBRARIES_DEBUG
    debug     ${DCMTK_dcmqrdb_LIBRARY_DEBUG}
    ${DCMTK_LIBRARIES_DEBUG}
    )

endif(DCMTK_dcmqrdb_LIBRARY)

if(DCMTK_dcmnet_LIBRARY)

  find_path( DCMTK_dcmnet_INCLUDE_DIR assoc.h
    ${DCMTK_DIR}/dcmnet/include
    ${DCMTK_DIR}/include/dcmnet
    ${DCMTK_DIR}/include/dcmtk/dcmnet
  )

  if(NOT DCMTK_dcmnet_INCLUDE_DIR) 
    find_path( DCMTK_dcmnet_INCLUDE_DIR dcmtk/dcmnet/assoc.h
      ${DCMTK_SOURCE_DIR}/dcmnet/include
    )
  endif(NOT DCMTK_dcmnet_INCLUDE_DIR)

  set( DCMTK_INCLUDE_DIR
    ${DCMTK_dcmnet_INCLUDE_DIR}
    ${DCMTK_INCLUDE_DIR}
    )

  set( DCMTK_LIBRARIES
    ${OPTIMIZED} ${DCMTK_dcmnet_LIBRARY}
    ${DCMTK_LIBRARIES}
    )
  set( DCMTK_LIBRARIES_DEBUG
    debug ${DCMTK_dcmnet_LIBRARY_DEBUG}
    ${DCMTK_LIBRARIES_DEBUG}
    )

    if (UNIX)
    set( DCMTK_LIBRARIES
    ${DCMTK_LIBRARIES}
    wrap
    )
    endif (UNIX)
endif(DCMTK_dcmnet_LIBRARY)

if( WIN32 )
  set( DCMTK_LIBRARIES ${DCMTK_LIBRARIES} netapi32 )
  if ( DCMTK_dcmdata_LIBRARY_DEBUG )
      set (DCMTK_LIBRARIES
          ${DCMTK_LIBRARIES}
          ${DCMTK_LIBRARIES_DEBUG})
  endif( DCMTK_dcmdata_LIBRARY_DEBUG)
endif( WIN32 )

endif( DCMTK_root_INCLUDE_DIR
    AND DCMTK_config_INCLUDE_DIR 
    AND DCMTK_ofstd_INCLUDE_DIR
    AND DCMTK_ofstd_LIBRARY
    AND DCMTK_dcmdata_INCLUDE_DIR
    AND DCMTK_dcmdata_LIBRARY
    AND DCMTK_dcmimgle_INCLUDE_DIR
    AND DCMTK_dcmimgle_LIBRARY
    AND DCMTK_dcmjpeg_LIBRARY
    AND DCMTK_dcmjpls_LIBRARY
    AND DCMTK_dcmjpls_INCLUDE_DIR)

#-----------------------------------------------------------------------------
if(NOT DCMTK_FOUND)

  set( DCMTK_DIR "" CACHE PATH ${DCMTK_DIR_MESSAGE} )
  set( DCMTK_SOURCE_DIR "" CACHE PATH ${DCMTK_SOURCE_DIR_MESSAGE} )

  # DCMTK not found, explain to the user how to specify its location.
  if(DCMTK2_FIND_REQUIRED)
    message(FATAL_ERROR ${DCMTK_DIR_MESSAGE})
  else(DCMTK2_FIND_REQUIRED)
    if(NOT DCMTK2_FIND_QUIETLY)
      message(STATUS ${DCMTK_DIR_MESSAGE})
    endif(NOT DCMTK2_FIND_QUIETLY)
  endif(DCMTK2_FIND_REQUIRED)
  

endif()
