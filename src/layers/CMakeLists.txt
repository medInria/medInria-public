###############################################################################
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


## #############################################################################
## Add subdirectories
## #############################################################################

## be sure medCore  is first in the list, it will be used by the other lib.
add_subdirectory(medCoreLegacy)
add_subdirectory(medCore)
add_subdirectory(medComposer)

add_subdirectory(medTest)
add_subdirectory(medPacs)

add_subdirectory(medLog)
add_subdirectory(medImageIO)
add_subdirectory(medRegistration)

add_subdirectory(medVtkInria)
