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

UNSET(DCMTK_FOUND)

# Construct consitent error messages for use below.
SET(DCMTK_DIR_DESCRIPTION "directory containing DCMTK install files.  This is either the root of the build or install tree.")
SET(DCMTK_SOURCE_DIR_DESCRIPTION "directory containing DCMTK source files.  This is root of the source tree.")
SET(DCMTK_DIR_MESSAGE "DCMTK not found.  Set the DCMTK_DIR cmake cache entry to the ${DCMTK_DIR_DESCRIPTION}")
SET(DCMTK_SOURCE_DIR_MESSAGE "DCMTK not found.  Set the DCMTK_SOURCE_DIR cmake cache entry to the ${DCMTK_SOURCE_DIR_DESCRIPTION}")

SET(DCMTK_DIR $ENV{DCMTK_DIR})
SET(DCMTK_SOURCE_DIR $ENV{DCMTK_SOURCE_DIR})

FIND_PATH( DCMTK_root_INCLUDE_DIR dcmtk/config/osconfig.h
  PATHS
  ${DCMTK_DIR}/include
  ${DCMTK_DIR}/config/include
)

FIND_PATH( DCMTK_config_INCLUDE_DIR osconfig.h
  ${DCMTK_DIR}/config/include
  ${DCMTK_DIR}/include/config
  ${DCMTK_DIR}/include/dcmtk/config
  ${DCMTK_DIR}/config/include/dcmtk/config
)

FIND_PATH( DCMTK_ofstd_INCLUDE_DIR ofstdinc.h
  ${DCMTK_DIR}/ofstd/include
  ${DCMTK_DIR}/include/ofstd
  ${DCMTK_DIR}/include/dcmtk/ofstd
)

IF(NOT DCMTK_ofstd_INCLUDE_DIR) 
  FIND_PATH( DCMTK_ofstd_INCLUDE_DIR dcmtk/ofstd/ofstdinc.h
    ${DCMTK_SOURCE_DIR}/ofstd/include
  )
ENDIF(NOT DCMTK_ofstd_INCLUDE_DIR)

FIND_LIBRARY( DCMTK_ofstd_LIBRARY ofstd
  ${DCMTK_DIR}/ofstd/libsrc
  ${DCMTK_DIR}/ofstd/libsrc/Release
  ${DCMTK_DIR}/ofstd/Release
  ${DCMTK_DIR}/lib
)

FIND_LIBRARY( DCMTK_ofstd_LIBRARY_DEBUG ofstd
  ${DCMTK_DIR}/ofstd/libsrc/Debug
  ${DCMTK_DIR}/ofstd/Debug
  ${DCMTK_DIR}/lib
)

FIND_PATH( DCMTK_oflog_INCLUDE_DIR logger.h
  ${DCMTK_DIR}/oflog/include
  ${DCMTK_DIR}/include/oflog
  ${DCMTK_DIR}/include/dcmtk/oflog
)

IF(NOT DCMTK_oflog_INCLUDE_DIR) 
  FIND_PATH( DCMTK_oflog_INCLUDE_DIR dcmtk/oflog/logger.h
    ${DCMTK_SOURCE_DIR}/oflog/include
  )
ENDIF(NOT DCMTK_oflog_INCLUDE_DIR)

FIND_LIBRARY( DCMTK_oflog_LIBRARY oflog
  ${DCMTK_DIR}/oflog/libsrc
  ${DCMTK_DIR}/oflog/libsrc/Release
  ${DCMTK_DIR}/oflog/Release
  ${DCMTK_DIR}/lib
)

FIND_LIBRARY( DCMTK_oflog_LIBRARY_DEBUG oflog
  ${DCMTK_DIR}/oflog/libsrc/Debug
  ${DCMTK_DIR}/oflog/Debug
)

FIND_PATH( DCMTK_dcmdata_INCLUDE_DIR dctypes.h
  ${DCMTK_DIR}/dcmdata/include
  ${DCMTK_DIR}/include/dcmdata
  ${DCMTK_DIR}/include/dcmtk/dcmdata
)

IF(NOT DCMTK_dcmdata_INCLUDE_DIR) 
  FIND_PATH( DCMTK_dcmdata_INCLUDE_DIR dcmtk/dcmdata/dctypes.h
    ${DCMTK_SOURCE_DIR}/dcmdata/include
  )
ENDIF(NOT DCMTK_dcmdata_INCLUDE_DIR)

FIND_LIBRARY( DCMTK_dcmdata_LIBRARY dcmdata
  ${DCMTK_DIR}/dcmdata/libsrc
  ${DCMTK_DIR}/dcmdata/libsrc/Release
  ${DCMTK_DIR}/dcmdata/Release
  ${DCMTK_DIR}/lib
)

FIND_LIBRARY( DCMTK_dcmdata_LIBRARY_DEBUG dcmdata
  ${DCMTK_DIR}/dcmdata/libsrc/Debug
  ${DCMTK_DIR}/dcmdata/Debug
)

FIND_PATH( DCMTK_dcmimgle_INCLUDE_DIR dcmimage.h
  ${DCMTK_DIR}/dcmimgle/include
  ${DCMTK_DIR}/include/dcmimgle
  ${DCMTK_DIR}/include/dcmtk/dcmimgle
)

IF(NOT DCMTK_dcmimgle_INCLUDE_DIR) 
  FIND_PATH( DCMTK_dcmimgle_INCLUDE_DIR dcmtk/dcmimgle/dcmimage.h
    ${DCMTK_SOURCE_DIR}/dcmimgle/include
  )
ENDIF(NOT DCMTK_dcmimgle_INCLUDE_DIR)

FIND_LIBRARY( DCMTK_dcmimgle_LIBRARY dcmimgle
  ${DCMTK_DIR}/dcmimgle/libsrc
  ${DCMTK_DIR}/dcmimgle/libsrc/Release
  ${DCMTK_DIR}/dcmimgle/Release
  ${DCMTK_DIR}/lib
)

FIND_LIBRARY( DCMTK_dcmimgle_LIBRARY_DEBUG dcmimgle
  ${DCMTK_DIR}/dcmimgle/libsrc/Debug
  ${DCMTK_DIR}/dcmimgle/Debug
)

FIND_PATH( DCMTK_dcmjpeg_INCLUDE_DIR djdecode.h
  ${DCMTK_DIR}/dcmjpeg/include
  ${DCMTK_DIR}/include/dcmjpeg
  ${DCMTK_DIR}/include/dcmtk/dcmjpeg
)

IF(NOT DCMTK_dcmjpeg_INCLUDE_DIR) 
  FIND_PATH( DCMTK_dcmjpeg_INCLUDE_DIR dcmtk/dcmjpeg/djdecode.h
    ${DCMTK_SOURCE_DIR}/dcmjpeg/include
  )
ENDIF(NOT DCMTK_dcmjpeg_INCLUDE_DIR)

FIND_LIBRARY( DCMTK_dcmjpeg_LIBRARY dcmjpeg
  ${DCMTK_DIR}/dcmjpeg/libsrc
  ${DCMTK_DIR}/dcmjpeg/libsrc/Release
  ${DCMTK_DIR}/dcmjpeg/Release
  ${DCMTK_DIR}/lib
)

FIND_LIBRARY( DCMTK_dcmjpeg_LIBRARY_DEBUG dcmjpeg
  ${DCMTK_DIR}/dcmjpeg/libsrc/Debug
  ${DCMTK_DIR}/dcmjpeg/Debug
)

FIND_LIBRARY( DCMTK_ijg8_LIBRARY ijg8
  ${DCMTK_DIR}/dcmjpeg/libijg8/
  ${DCMTK_DIR}/dcmjpeg/libijg8/Release
  ${DCMTK_DIR}/lib
)

FIND_LIBRARY( DCMTK_ijg8_LIBRARY_DEBUG ijg8
  ${DCMTK_DIR}/dcmjpeg/libijg8/Debug
)

FIND_LIBRARY( DCMTK_ijg12_LIBRARY ijg12
  ${DCMTK_DIR}/dcmjpeg/libijg12/
  ${DCMTK_DIR}/dcmjpeg/libijg12/Release
  ${DCMTK_DIR}/lib
)

FIND_LIBRARY( DCMTK_ijg12_LIBRARY_DEBUG ijg12
  ${DCMTK_DIR}/dcmjpeg/libijg12/Debug
)

FIND_LIBRARY( DCMTK_ijg16_LIBRARY ijg16
  ${DCMTK_DIR}/dcmjpeg/libijg16/
  ${DCMTK_DIR}/dcmjpeg/libijg16/Release
  ${DCMTK_DIR}/lib
)

FIND_LIBRARY( DCMTK_ijg16_LIBRARY_DEBUG ijg16
  ${DCMTK_DIR}/dcmjpeg/libijg16/Debug
)

FIND_LIBRARY(DCMTK_dcmqrdb_LIBRARY dcmqrdb 
  ${DCMTK_DIR}/dcmqrdb/libsrc/Release
  ${DCMTK_DIR}/dcmqrdb/libsrc/
  ${DCMTK_DIR}/lib
)

FIND_LIBRARY(DCMTK_dcmqrdb_LIBRARY_DEBUG dcmqrdb 
  ${DCMTK_DIR}/dcmqrdb/libsrc/Debug
)

FIND_LIBRARY(DCMTK_dcmnet_LIBRARY dcmnet 
  ${DCMTK_DIR}/dcmnet/libsrc/Release
  ${DCMTK_DIR}/dcmnet/libsrc/
  ${DCMTK_DIR}/lib
)

FIND_LIBRARY(DCMTK_dcmnet_LIBRARY_DEBUG dcmnet 
  ${DCMTK_DIR}/dcmnet/libsrc/Debug
)


IF( DCMTK_root_INCLUDE_DIR
    AND DCMTK_config_INCLUDE_DIR 
    AND DCMTK_ofstd_INCLUDE_DIR 
    AND DCMTK_ofstd_LIBRARY
    AND DCMTK_dcmdata_INCLUDE_DIR
    AND DCMTK_dcmdata_LIBRARY
    AND DCMTK_dcmimgle_INCLUDE_DIR
    AND DCMTK_dcmimgle_LIBRARY 
    AND DCMTK_dcmjpeg_LIBRARY )

  SET( DCMTK_FOUND 1)

  SET( DCMTK_INCLUDE_DIR
    ${DCMTK_root_INCLUDE_DIR}
    ${DCMTK_config_INCLUDE_DIR}
    ${DCMTK_ofstd_INCLUDE_DIR}
    ${DCMTK_dcmdata_INCLUDE_DIR}
    ${DCMTK_dcmimgle_INCLUDE_DIR}
    ${DCMTK_dcmjpeg_INCLUDE_DIR}
  )
  IF( WIN32 )
    set (OPTIMIZED optimized)
  endif(WIN32)

  SET( DCMTK_LIBRARIES
    ${OPTIMIZED} ${DCMTK_dcmjpeg_LIBRARY}
    ${OPTIMIZED} ${DCMTK_ijg8_LIBRARY}
    ${OPTIMIZED} ${DCMTK_ijg12_LIBRARY}
    ${OPTIMIZED} ${DCMTK_ijg16_LIBRARY}
    ${OPTIMIZED} ${DCMTK_dcmimgle_LIBRARY}
    ${OPTIMIZED} ${DCMTK_dcmdata_LIBRARY}
  )
  SET (DCMTK_LIBRARIES_DEBUG
    debug     ${DCMTK_dcmjpeg_LIBRARY_DEBUG}
    debug     ${DCMTK_ijg8_LIBRARY_DEBUG}
    debug     ${DCMTK_ijg12_LIBRARY_DEBUG}
    debug     ${DCMTK_ijg16_LIBRARY_DEBUG}
    debug     ${DCMTK_dcmimgle_LIBRARY_DEBUG}
    debug     ${DCMTK_dcmdata_LIBRARY_DEBUG}
  )

  IF ( DCMTK_oflog_LIBRARY )#oflog only in dcmtk>=3.5.5
    SET( DCMTK_INCLUDE_DIR
      ${DCMTK_oflog_INCLUDE_DIR}
      ${DCMTK_INCLUDE_DIR}
    )
    SET( DCMTK_LIBRARIES
      ${DCMTK_LIBRARIES}
      ${OPTIMIZED} ${DCMTK_oflog_LIBRARY} #must be defined before ofstd
    )
    SET( DCMTK_LIBRARIES_DEBUG
      ${DCMTK_LIBRARIES_DEBUG}
      debug     ${DCMTK_oflog_LIBRARY_DEBUG} #must be defined before ofstd 
    )

  ENDIF ( DCMTK_oflog_LIBRARY )
  
  SET( DCMTK_LIBRARIES 
   ${DCMTK_LIBRARIES}
   ${OPTIMIZED} ${DCMTK_ofstd_LIBRARY}
#   ${OPTIMIZED} ${DCMTK_config_LIBRARY}    
  )
  SET( DCMTK_LIBRARIES_DEBUG 
   ${DCMTK_LIBRARIES_DEBUG}
   debug     ${DCMTK_ofstd_LIBRARY_DEBUG}
#  debug     ${DCMTK_config_LIBRARY_DEBUG}    
  )


  IF(NOT WIN32)
    SET( DCMTK_LIBRARIES
    ${DCMTK_LIBRARIES}
    z
    )   
  ENDIF(NOT WIN32)
  
IF(DCMTK_dcmqrdb_LIBRARY)
  SET( DCMTK_LIBRARIES
    ${OPTIMIZED} ${DCMTK_dcmqrdb_LIBRARY}
    ${DCMTK_LIBRARIES}
    )
  SET( DCMTK_LIBRARIES_DEBUG
    debug     ${DCMTK_dcmqrdb_LIBRARY_DEBUG}
    ${DCMTK_LIBRARIES_DEBUG}
    )

ENDIF(DCMTK_dcmqrdb_LIBRARY)

IF(DCMTK_dcmnet_LIBRARY)

  FIND_PATH( DCMTK_dcmnet_INCLUDE_DIR assoc.h
    ${DCMTK_DIR}/dcmnet/include
    ${DCMTK_DIR}/include/dcmnet
    ${DCMTK_DIR}/include/dcmtk/dcmnet
  )

  IF(NOT DCMTK_dcmnet_INCLUDE_DIR) 
    FIND_PATH( DCMTK_dcmnet_INCLUDE_DIR dcmtk/dcmnet/assoc.h
      ${DCMTK_SOURCE_DIR}/dcmnet/include
    )
  ENDIF(NOT DCMTK_dcmnet_INCLUDE_DIR)

  SET( DCMTK_INCLUDE_DIR
    ${DCMTK_dcmnet_INCLUDE_DIR}
    ${DCMTK_INCLUDE_DIR}
    )

  SET( DCMTK_LIBRARIES
    ${OPTIMIZED} ${DCMTK_dcmnet_LIBRARY}
    ${DCMTK_LIBRARIES}
    )
  SET( DCMTK_LIBRARIES_DEBUG
    debug ${DCMTK_dcmnet_LIBRARY_DEBUG}
    ${DCMTK_LIBRARIES_DEBUG}
    )

    if (UNIX)
    SET( DCMTK_LIBRARIES
    ${DCMTK_LIBRARIES}
    wrap
    )
    endif (UNIX)
ENDIF(DCMTK_dcmnet_LIBRARY)

IF( WIN32 )
  SET( DCMTK_LIBRARIES ${DCMTK_LIBRARIES} netapi32 )
  IF ( DCMTK_dcmdata_LIBRARY_DEBUG )
      SET (DCMTK_LIBRARIES
          ${DCMTK_LIBRARIES}
          ${DCMTK_LIBRARIES_DEBUG})
  ENDIF( DCMTK_dcmdata_LIBRARY_DEBUG)
ENDIF( WIN32 )

ENDIF( DCMTK_root_INCLUDE_DIR
    AND DCMTK_config_INCLUDE_DIR 
    AND DCMTK_ofstd_INCLUDE_DIR
    AND DCMTK_ofstd_LIBRARY
    AND DCMTK_dcmdata_INCLUDE_DIR
    AND DCMTK_dcmdata_LIBRARY
    AND DCMTK_dcmimgle_INCLUDE_DIR
    AND DCMTK_dcmimgle_LIBRARY
    AND DCMTK_dcmjpeg_LIBRARY )

#-----------------------------------------------------------------------------
IF(DCMTK_FOUND)
  
ELSE(DCMTK_FOUND)

  SET( DCMTK_DIR "" CACHE PATH ${DCMTK_DIR_MESSAGE} )
  SET( DCMTK_SOURCE_DIR "" CACHE PATH ${DCMTK_SOURCE_DIR_MESSAGE} )
    
  # DCMTK not found, explain to the user how to specify its location.
  IF(DCMTK2_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR ${DCMTK_DIR_MESSAGE})
  ELSE(DCMTK2_FIND_REQUIRED)
    IF(NOT DCMTK2_FIND_QUIETLY)
      MESSAGE(STATUS ${DCMTK_DIR_MESSAGE})
    ENDIF(NOT DCMTK2_FIND_QUIETLY)
  ENDIF(DCMTK2_FIND_REQUIRED)
  

ENDIF(DCMTK_FOUND)
