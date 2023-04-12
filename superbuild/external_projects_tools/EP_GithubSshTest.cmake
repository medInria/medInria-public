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
## Add github.com's SSH signature to the .ssh/known_hosts file
## #############################################################################

set(GITHUB_SIGN "github.com ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQCj7ndNxQowgcQnjshcLrqPEiiphnt+VTTvDP6mHBL9j1aNUkY4Ue1gvwnGLVlOhGeYrnZaMgRK6+PKCUXaDbC7qtbW8gIkhL7aGCsOr/C56SJMy/BCZfxd1nWzAOxSDPgVsmerOBYfNqltV9/hWCqBywINIR+5dIg6JTJ72pcEpEjcYgXkE2YEFXV1JHnsKgbLWNlhScqb2UmyRkQyytRLtL+38TGxkxCflmO+5Z8CSSNY7GidjMIZ7Q4zMjA2n1nGrlTDkzwDCsw+wqFPGQA179cnfGWOWRVruj16z6XyvxvjJwbz0wQZ75XK5tKSb7FNyeIEs4TT4jk+S4dhPeAUC5y+bDYirYgM4GC7uEnztnZyaVWQ7B381AK4Qdrwt51ZqExKbQpTUNn+EjqoTwvqNj4kqx5QUCI0ThS/YkOxJCXmPUWZbhjpCg56i+2aB6CmK2JGhn57K5mj0MNdBXA4/WnwH6XoPWJzK5Nyu2zB3nAZp+S5hpQs+p1vN1/wsjk=")

file(TO_CMAKE_PATH ${HOME_PATH} HOME_PATH)
set(SSH_KNOWN_HOSTS_PATH ${HOME_PATH}/.ssh/known_hosts)
file(STRINGS ${SSH_KNOWN_HOSTS_PATH} KNOWN_HOSTS REGEX github\\.com)
list(LENGTH KNOWN_HOSTS N)
set (INDEX 0)

while(INDEX LESS N)
  list(GET KNOWN_HOSTS ${INDEX} KNOWN_HOST)
  string(STRIP ${KNOWN_HOST} KNOWN_HOST)
  string(COMPARE EQUAL ${KNOWN_HOST} ${GITHUB_SIGN} GITHUB_FOUND)
  if (GITHUB_FOUND)
	  message(STATUS "Found Github's SSH signature in ${SSH_KNOWN_HOSTS_PATH}")
	  break()
  endif()
  math(EXPR INDEX "${INDEX} + 1" )
endwhile()

if ( NOT GITHUB_FOUND)
  message(STATUS 
    "Could not find Github's SSH signature, appending to ${SSH_KNOWN_HOSTS_PATH}..." 
    )
  file(APPEND ${SSH_KNOWN_HOSTS_PATH} "\n${GITHUB_SIGN}\n")
endif()


## #############################################################################
## Make sure the user's key has been upload to GitHub
## #############################################################################

message(STATUS "Testing user's access to GitHub...")
execute_process(COMMAND ${SSH_BIN} -T git@github.com
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
    "Could not connect to GitHub using SSH public key (found here : ${SSH_PUB_KEY}) 
    Add this key to your GitHub account ( https://github.com/settings/ssh ) : 
    ${SSH_PUB_KEY_VALUE}" 
    )
else()
  message( STATUS "GitHub access successful !" )
endif()

set (SKIP_GITHUB_TESTS ON CACHE BOOL 
  "GitHub access tests were successful, set ON to force re-evaluation" 
  FORCE
  )


