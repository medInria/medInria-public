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


# # argument LIST is a list with plugin names at odd indices followed by
# # the respective default build values (ON/OFF) at even indices
# MACRO(MACRO_ADD_PLUGIN_LIST P_LIST)
  
#   message("${P_LIST}")
#   # list(LENGTH "${P_LIST}" N)
#   list(LENGTH P_LIST N)
#   message("${N}")
#   while(N GREATER 0)
#     list(GET P_LIST 0 PLUGIN_ITEM)
#     list(GET P_LIST 1 BUILD_VALUE)
#     if (NOT ${BUILD_VALUE} MATCHES "O(N|FF)")
#       message ("The even members of the plugin list must be ON or OFF")
#     endif()

#     if (MEDINRIA_BUILD_ALL_PLUGINS)
#       set(BUILD_VALUE ON)
#     endif()

#     MACRO_ADD_PLUGIN (${PLUGIN_ITEM} ${BUILD_VALUE})
#     message(${PLUGIN_ITEM} " " ${BUILD_VALUE})

#     list(REMOVE_AT P_LIST 0 1)
#     list(LENGTH P_LIST N)
#   endwhile()

# ENDMACRO(MACRO_ADD_PLUGIN_LIST)
