################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

FIND_PATH(QTDCM_DIR UseQtDCM.cmake 
  /usr/lib/qtdcm-0.1 
  /usr/local/lib/qtdcm-0.1
  DOC "QTDCM directory")

SET(QTDCM_CONFIG_FILE ${QTDCM_DIR}/QtDCMConfig.cmake)
SET(QTDCM_USE_FILE ${QTDCM_DIR}/UseQtDCM.cmake)
# path is okay
IF (EXISTS ${QTDCM_CONFIG_FILE})
  SET(QTDCM_FOUND 1)
  INCLUDE (${QTDCM_CONFIG_FILE})
  INCLUDE (${QTDCM_USE_FILE})
ELSE (EXISTS ${QTDCM_CONFIG_FILE})
  SET(QTDCM_FOUND 0)
  IF(QTDCM_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Please set QTDCM_DIR to the correct value")
  ENDIF(QTDCM_FIND_REQUIRED)
ENDIF (EXISTS ${QTDCM_CONFIG_FILE})
