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
  # GitBash
  find_program(BASH_BIN NAMES bash)
  if (NOT BASH_BIN)
    if(EXISTS $ENV{ProgramFiles}/Git/git-bash.exe)
	  set(BASH_BIN "$ENV{ProgramFiles}/Git/git-bash.exe" CACHE FILEPATH "Path to git bash program" FORCE)
	endif()
  endif()
  
  if (NOT BASH_BIN)
    message( SEND_ERROR "You need to install GitBash and add it to the PATH environment variable.")
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
  message(SEND_ERROR  "You need to install Git and add it to the PATH environment variable.")
else()
  mark_as_advanced(GIT_BIN)
endif()

## #############################################################################
## Test ssh access for github
## #############################################################################

set (SKIP_GITHUB_TESTS OFF CACHE BOOL "Set this to ON to skip GitHub access tests")
mark_as_advanced(SKIP_GITHUB_TESTS)

if (NOT ${SKIP_GITHUB_TESTS} AND ${USE_GITHUB_SSH})
  ## #############################################################################
  ## Look for SSH
  ## #############################################################################

  find_program(SSH_BIN NAMES ssh)
  if (NOT SSH_BIN)
    message(SEND_ERROR "You need to install SSH and add it to the PATH environment variable.")
  else()
    mark_as_advanced(SSH_BIN)
  endif()
  include(EP_GithubSshTest)
endif()



## #############################################################################
## Test ssh access for gitlab Inria
## #############################################################################

set (SKIP_GITLAB_INRIA_TESTS OFF CACHE BOOL 
  "Set this to ON to skip GitLab Inria access tests"
  )
mark_as_advanced(SKIP_GITLAB_INRIA_TESTS)

if (NOT ${SKIP_GITLAB_INRIA_TESTS} AND ${USE_GITLAB_INRIA_SSH})
  ## #############################################################################
  ## Look for SSH
  ## #############################################################################

  find_program(SSH_BIN NAMES ssh)
  if (NOT SSH_BIN)
    message(SEND_ERROR "You need to install SSH and add it to the PATH environment variable.")
  else()
    mark_as_advanced(SSH_BIN)
  endif()
  include(EP_GitlabInriaSshTest)
endif()