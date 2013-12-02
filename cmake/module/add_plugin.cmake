#######################################################################
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
#######################################################################

macro(add_plugin  plugin_name default_build_value)
  string(TOUPPER ${plugin_name} upper_case_name)
  
  option(BUILD_${upper_case_name}_PLUGIN 
    "Build ${plugin_name} plugin" 
    ${default_build_value}
    )
  
  if (BUILD_ALL_PLUGINS)
  # Force to update the cache
    set(BUILD_${upper_case_name}_PLUGIN ON 
      CACHE BOOL "Build ${plugin_name} plugin" FORCE
      )
  endif()

  if (BUILD_ALL_PLUGINS OR BUILD_${upper_case_name}_PLUGIN)
    add_subdirectory(${plugin_name})
  endif()
endmacro()


macro(add_plugins plugins_list)
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
