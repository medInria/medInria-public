MACRO(FixDCMTKMacInstall)

  foreach(lib dcmdata dcmimage dcmimgle dcmjpeg dcmnet dcmpstat dcmqrdb dcmsr dcmtls ijg12 ijg16 ijg8 oflog ofstd)
    foreach(linkedlib dcmdata dcmimage dcmimgle dcmjpeg dcmnet dcmpstat dcmqrdb dcmsr dcmtls ijg12 ijg16 ijg8 oflog ofstd)
      execute_process(COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change lib${linkedlib}.dylib ${DCMTK_DIR}/lib/lib${linkedlib}.dylib ${DCMTK_DIR}/lib/lib${lib}.dylib)
    endforeach()
  endforeach()

ENDMACRO(FixDCMTKMacInstall)
