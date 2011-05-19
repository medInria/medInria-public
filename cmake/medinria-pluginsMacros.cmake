###############################################################################
#
# Macros used by medinria-plugins
#
# Author: Benoît Bleuzé
#
###############################################################################

MACRO(MACRO_ADD_PLUGIN  P_NAME DEFAULT_BUILD_VALUE)

string(TOUPPER ${P_NAME} UPPER_CASE_NAME)
# set(MEDINRIA_BUILD_${UPPER_CASE_NAME}_PLUGIN ${DEFAULT_BUILD_VALUE})
option(MEDINRIA_BUILD_${UPPER_CASE_NAME}_PLUGIN "Build ${P_NAME} plugin" "${DEFAULT_BUILD_VALUE}")

if (MEDINRIA_BUILD_ALL_PLUGINS)
    set(MEDINRIA_BUILD_${UPPER_CASE_NAME}_PLUGIN "ON")
endif()

if (MEDINRIA_BUILD_ALL_PLUGINS OR
    MEDINRIA_BUILD_${UPPER_CASE_NAME}_PLUGIN )
  add_subdirectory(${P_NAME})
endif()


ENDMACRO(MACRO_ADD_PLUGIN)
