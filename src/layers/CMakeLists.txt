################################################################################
#
# medInria

# Copyright (c) INRIA 2013. All rights reserved.

# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

## #############################################################################
## Add subdirectories
## #############################################################################

## be sure medCore  is first in the list, it will be used by the other lib.
add_subdirectory(legacy/medCoreLegacy)
add_subdirectory(medCore)
add_subdirectory(medWidgets)
add_subdirectory(medComposer)

add_subdirectory(legacy/medPacs)

add_subdirectory(legacy/medLog)
add_subdirectory(legacy/medImageIO)
add_subdirectory(legacy/medRegistration)

add_subdirectory(legacy/medVtkInria)
add_subdirectory(legacy/medUtilities)
add_subdirectory(legacy/medVtkDataMeshBase)

if (USE_PYTHON)
    add_subdirectory(medPython)
endif()
