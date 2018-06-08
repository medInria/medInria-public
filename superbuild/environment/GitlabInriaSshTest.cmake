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

## #############################################################################
## Add gitlab.inria.fr's SSH signature to the .ssh/known_hosts file
## #############################################################################

set(GITLAB_INRIA_SIGN "gitlab.inria.fr ssh-rsa AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBAFldY2ft1ReZjFYPpe/wa5Vhl4YJfYz6IJOZaZxL924R44jrGWiR2/Misrug75NBsQB+UWs1iIyWZPk0AN45Sk="
  )

file(TO_CMAKE_PATH ${HOME_PATH} HOME_PATH)
set(SSH_KNOWN_HOSTS_PATH ${HOME_PATH}/.ssh/known_hosts)
file(STRINGS ${SSH_KNOWN_HOSTS_PATH} KNOWN_HOSTS REGEX gitlab\\.inria\\.fr)
list(LENGTH KNOWN_HOSTS N)
set (INDEX 0)

while(INDEX LESS N)
  list(GET KNOWN_HOSTS ${INDEX} KNOWN_HOST)
  string(STRIP ${KNOWN_HOST} KNOWN_HOST)
  string(COMPARE EQUAL ${KNOWN_HOST} ${GITLAB_INRIA_SIGN} GITLAB_INRIA_FOUND)
  if (GITLAB_INRIA_FOUND)
	  message(STATUS "Found Gitlab Inria's SSH signature in ${SSH_KNOWN_HOSTS_PATH}")
	  break()
  endif()
  math(EXPR INDEX "${INDEX} + 1" )
endwhile()

if ( NOT GITLAB_INRIA_FOUND)
  message(STATUS 
    "Could not find Gitlab Inria's SSH signature, appending to ${SSH_KNOWN_HOSTS_PATH}..." 
    )
  file(APPEND ${SSH_KNOWN_HOSTS_PATH} "\n${GITLAB_INRIA_SIGN}\n")
endif()


## #############################################################################
## Make sure the user's key has been upload to Gitlab Inria
## #############################################################################

message(STATUS "Testing user's access to Gitlab Inria...")
execute_process(COMMAND ${SSH_BIN} -T git@gitlab.inria.fr
  TIMEOUT 10
  RESULT_VARIABLE SSH_TEST_RESULT
  OUTPUT_QUIET
  ERROR_QUIET
  )


## #############################################################################
## Test for a SSH key
## #############################################################################

if (SSH_TEST_RESULT EQUAL 255) 
  set(SSH_PUB_KEY ${HOME_PATH}/.ssh/id_rsa.pub)
  if (NOT EXISTS ${SSH_PUB_KEY})
	  message(FATAL_ERROR 
	    "Could not find a public SSH key, you need to generate one." 
	    )
  else()
	  message(STATUS 
	    "Found the user's public SSH key..." 
	    )
  endif()

  file(READ ${SSH_PUB_KEY} SSH_PUB_KEY_VALUE)
  message(FATAL_ERROR 
    "Could not connect to Gitlab Inria using SSH public key (found here : ${SSH_PUB_KEY}) 
    Add this key to your Gitlab Inria account ( https://gitlab.inria.fr/profile/keys ) : 
    ${SSH_PUB_KEY_VALUE}" 
    )
else()
  message( STATUS "Gitlab Inria access successful !" )
endif()

set (SKIP_GITLAB_INRIA_TESTS ON CACHE BOOL 
  "Gitlab Inria access tests were successful, set ON to force re-evaluation" 
  FORCE
  )


