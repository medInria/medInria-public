


MACRO( MAKE_NSIS_INSTALLER TARGET INPUT VERSION)

  FIND_FILE(MAKENSIS makensis.exe
  "C:/Program Files/NSIS/"        
  )                
                      
  IF( MAKENSIS )
         
   FILE(TO_CMAKE_PATH "$ENV{SYSTEMROOT}" SYSTEMROOT)

    IF(CMAKE_CL_64)
      SET(MSVC_ARCH amd64)
    ELSE(CMAKE_CL_64)
      SET(MSVC_ARCH x86)
    ENDIF(CMAKE_CL_64)
   
   SET(NSIS_OPTIONS
   ${NSIS_OPTIONS}
   "/DmedularDIR=${EXECUTABLE_OUTPUT_PATH}"
   "/DmedularLIBDIR=${LIBRARY_OUTPUT_PATH}/release"
   "/DVERSION=${VERSION}"
   "/DSRCDIR=${PROJECT_SOURCE_DIR}"
   "/DINST_PREFIX=${CMAKE_INSTALL_PREFIX}"
   )

#   IF( MSVC90 )
#     FIND_PATH(MSVC90_REDIST_DIR NAMES ${MSVC_ARCH}/Microsoft.VC90.CRT/Microsoft.VC90.CRT.manifest
#     PATHS "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0;InstallDir]/../../VC/redist"
#     )
#     MARK_AS_ADVANCED(MSVC90_REDIST_DIR)
#     SET(MSVDIR "${MSVC90_REDIST_DIR}/${MSVC_ARCH}/Microsoft.VC90.CRT")
#     # FIND_PATH(MSVDIR msvcp90.dll "C:/windows/system32")
#   ENDIF( MSVC90 )
#
#   IF( MSVC80 )
#     FIND_PATH(MSVC80_REDIST_DIR NAMES ${MSVC_ARCH}/Microsoft.VC80.CRT/Microsoft.VC80.CRT.manifest
#     PATHS "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0;InstallDir]/../../VC/redist"
#     )
#     MARK_AS_ADVANCED(MSVC80_REDIST_DIR)
#     SET(MSVDIR "${MSVC80_REDIST_DIR}/${MSVC_ARCH}/Microsoft.VC80.CRT")
#     #FIND_PATH(MSVDIR msvcp80.dll "C:/windows/system32")
#   ENDIF( MSVC80 )
#	   
#   #IF( MSVC71 )
#   #  FIND_PATH(MSVDIR msvcp71.dll "C:/windows/system32")
#   #ENDIF( MSVC71 )
#       
#   #IF( MSVC70 )
#   #  FIND_PATH(MSVDIR msvcp70.dll "C:/windows/system32")
#   #ENDIF( MSVC70 )
#       
#   IF( MSVC60 OR MSVC70 OR MSVC71 )
#     SET(MSVDIR "${SYSTEMROOT}/system32")
#     # FIND_PATH(MSVDIR msvcp60.dll "C:/windows/system32")
#   ENDIF( MSVC60 OR MSVC70 OR MSVC71 )
#            

#   IF(CMAKE_CL_64)
#      SET(NSIS_OPTIONS
#      ${NSIS_OPTIONS}
#      "/DWIN64"			 
#      )           
#    ENDIF(CMAKE_CL_64)


       ADD_CUSTOM_COMMAND(
       TARGET ${TARGET} POST_BUILD
       COMMAND ${MAKENSIS} 
       ARGS ${NSIS_OPTIONS} ${INPUT}
       )
       
     ENDIF( MAKENSIS )

ENDMACRO( MAKE_NSIS_INSTALLER )

IF(WIN32)
    
    ADD_CUSTOM_TARGET(nsis  
	COMMENT "Create an installer for windows"
	DEPENDS install 
	)
    MAKE_NSIS_INSTALLER( nsis  ${PROJECT_SOURCE_DIR}/installerMedular.nsi  ${${PROJECT_NAME}_VERSION})
ENDIF(WIN32)



