################################################################################
#
# medInria
#
# Copyright (c) INRIA 2021. All rights reserved.
#
# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

if (UNIX)
    if (APPLE)
        include(python_files_info_mac)
    else()
        include(python_files_info_linux)
    endif()
elseif (WIN32)
    include(python_files_info_win)
endif()
