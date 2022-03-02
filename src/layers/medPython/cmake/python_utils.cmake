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

set(PYTHON_PROJECT_NAME medPython)
set(PYTHON_PLUGIN_PREFIX ${PROJECT_NAME}_)

cmake_policy(SET CMP0078 NEW)
cmake_policy(SET CMP0086 NEW)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include(embed_python)
include(add_python_bindings)
include(add_python_modules)
include(generate_python_resources)
