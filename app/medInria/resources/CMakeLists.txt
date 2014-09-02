################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################
file(GLOB ${PROJECT_NAME}_QRC
*.qrc
)
set(${PROJECT_NAME}_QRC
${${PROJECT_NAME}_QRC}
PARENT_SCOPE
)
file(GLOB ${PROJECT_NAME}_QSS
*.qss
)
set(${PROJECT_NAME}_QSS
${${PROJECT_NAME}_QSS}
PARENT_SCOPE
)
