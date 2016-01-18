#=========================================================================
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
#=========================================================================

#
# See CMake/ctkFunctionGetTargetLibraries.cmake
# 
# This file should list the libraries required to build the current CTK application.
# 

set(target_libraries
  CTKCommandLineModulesFrontendQtGui
  CTKCommandLineModulesBackendLocalProcess
  CTKCommandLineModulesBackendFunctionPointer
  CTKWidgets
  )
