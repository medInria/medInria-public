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

project(GetRevisions)
cmake_minimum_required(VERSION 2.8)

## #############################################################################
## A dummy project configured during an additional step of the medInria project.
## It retreives the SHA1 and Revision of git and svn based projects,
## and write the version of all the projects - medinria and its dependeces -
## into as a macro in medRevisions.h located in the Binary dir of medinria.
## ############################################################################# 

set(REVISIONS_HEADER ${CMAKE_BINARY_DIR}/medRevisions.h)
file(WRITE ${REVISIONS_HEADER}
  "#pragma once\n#define REVISIONS \"<br/><br/>medInria was build using : <br/><br/>"
  )

## #############################################################################
## Qt4 version
## #############################################################################

execute_process(COMMAND qmake-qt4 --version
  COMMAND grep "Qt version"
  COMMAND sed s/Using\\s*//ig
  COMMAND sed s/in.*//ig
  OUTPUT_VARIABLE Qt4_revision
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ) 

file(APPEND ${REVISIONS_HEADER}
  "${Qt4_revision}<br/>"
  )

## #############################################################################
## Svn revision and git SHA1
## #############################################################################

set(CMAKE_MODULE_PATH 
  ${CMAKE_BINARY_DIR} 
  ${CMAKE_MODULE_PATH}
  )
include(revisions_args)

set(CMAKE_MODULE_PATH 
  ${GET_REVISIONS_MODULE_PATH}
  ${CMAKE_MODULE_PATH}
  )  
include(EP_GetGitSvnRevision)

EP_GetGitSvnRevision(PROJECT "${projects}"
  OUTPUT ${REVISIONS_HEADER}
  )


## #############################################################################
## Close with "
## #############################################################################
  
file(APPEND ${REVISIONS_HEADER}
  "\""
  )
  

