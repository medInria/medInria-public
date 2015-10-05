##############################################################################
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

# This file checks that all dependencies are met, and it necessary sets up a few
# things, like adding the github.com signature to known_hosts.

# Assume Unix by default
set(HOME_PATH $ENV{HOME})


## #############################################################################
## Look for windows specific programs
## #############################################################################

if (WIN32)
  # Check for Visual Studio 2008 or 2010 or 2012
  if (NOT MSVC9 AND NOT MSVC10 AND NOT MSVC11)
    message(SEND_ERROR 
      "MedInria can only be compiled with Visual Studio 2008 or 2010 or 2012 at this time." 
      )
  endif()

  # Check for DirectX SDK (for VTK)
  file(GLOB DIRECTX_SDK "C:/Program Files*/Microsoft DirectX SDK*")
  if (NOT DIRECTX_SDK)
    message(SEND_ERROR 
      "You need to install Microsoft DirectX SDK." 
      )
  else()
      mark_as_advanced(DIRECTX_SDK)
  endif()

  # GitBash
  find_program(BASH_BIN NAMES bash)
  if (NOT BASH_BIN)
    message( SEND_ERROR 
      "You need to install GitBash and add it to the PATH environment variable." 
      )
  else()
      mark_as_advanced(BASH_BIN)
  endif()

  set(HOME_PATH $ENV{HOMEDRIVE}$ENV{HOMEPATH})
endif()


## #############################################################################
## Look for Git
## #############################################################################
find_program(GIT_BIN NAMES git)
if (NOT GIT_BIN)
  message(SEND_ERROR 
    "You need to install Git and add it to the PATH environment variable." 
    )
else()
  mark_as_advanced(GIT_BIN)
endif()

## #############################################################################
## Look for SSH
## #############################################################################

find_program(SSH_BIN NAMES ssh)
if (NOT SSH_BIN)
  message(SEND_ERROR 
    "You need to install SSH and add it to the PATH environment variable."
    )
else()
  mark_as_advanced(SSH_BIN)
endif()



## #############################################################################
## Look for Subversion
## #############################################################################

find_program(SVN_BIN NAMES svn)
if (NOT SVN_BIN)
  message(SEND_ERROR 
    "You need to install Subversion and add it to the PATH environment variable." 
    )
else()
  mark_as_advanced(SVN_BIN)
endif()


## #############################################################################
## Test ssh access for github
## #############################################################################

set (SKIP_GITHUB_TESTS OFF CACHE BOOL 
  "Set this to ON to skip GitHub access tests"
  )
mark_as_advanced(SKIP_GITHUB_TESTS)

if (NOT ${SKIP_GITHUB_TESTS} AND ${USE_GITHUB_SSH})
  include(GithubSshTest)
endif()

