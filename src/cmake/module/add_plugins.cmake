################################################################################
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

macro(add_plugin  plugin_name default_build_value)

string(TOUPPER ${plugin_name} upper_case_name)

## #############################################################################
## Get uppercased name.
## #############################################################################

  option(BUILD_${upper_case_name}_PLUGIN
    "Build ${plugin_name} plugin"
    ${default_build_value}
    )
  mark_as_advanced(BUILD_${upper_case_name}_PLUGIN)


## #############################################################################
## Force to update the cache if we build all the plugins
## #############################################################################

  if(BUILD_ALL_PLUGINS)
    set(BUILD_${upper_case_name}_PLUGIN ON
      CACHE BOOL "Build ${plugin_name} plugin" FORCE
      )
  endif()

## #############################################################################
## Subdirs
## #############################################################################
  if(BUILD_ALL_PLUGINS OR BUILD_${upper_case_name}_PLUGIN)
    add_subdirectory(${plugin_name})
  endif()

endmacro()


################################################################################
#
# Usage: add_plugins(plugin1, def_value1, plugin2, def_value2, plugin3 ...)
# For each plugin, create an option in the cache with the value def_value,
# if it doesn't already exists.
# If the option is set to ON the plugin is subdirs.
#
################################################################################

macro(add_plugins plugins_list)

## #############################################################################
## Call add_plugin for each plugin
## #############################################################################

    set(plugin_list ${ARGV})
    set(index 0)

    while(index LESS ${ARGC})
        math(EXPR index_ "${index} + 1")
        list(GET plugin_list ${index} plugin_name)
        list(GET plugin_list ${index_} default_build_value)

        add_plugin(${plugin_name} ${default_build_value})

        math(EXPR index "${index} + 2")
    endwhile()

endmacro()
