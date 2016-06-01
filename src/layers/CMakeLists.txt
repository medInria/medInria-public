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
add_subdirectory(legacy/medCoreLegacy)
add_subdirectory(medCore)
add_subdirectory(medWidgets)
add_subdirectory(medComposer)


add_subdirectory(legacy/medTest)
add_subdirectory(legacy/medPacs)

add_subdirectory(legacy/medLog)
add_subdirectory(legacy/medImageIO)
add_subdirectory(legacy/medRegistration)

add_subdirectory(legacy/medVtkInria)

find_package(dtkImaging QUIET)

if(dtkImaging_FOUND)
    message("loading dtkImaging")
    add_subdirectory(medImagingCompatibility)
endif(dtkImaging_FOUND)
