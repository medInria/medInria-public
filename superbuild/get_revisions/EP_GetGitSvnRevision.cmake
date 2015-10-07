################################################################################
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
################################################################################
macro(EP_GetGitSvnRevision
  PROJECTS projects
  OUTPUT output
  )


foreach(project ${projects})
## #############################################################################
## Git SHA1
## #############################################################################

  if(EXISTS ${sp_source_dir}/${project}/.git)
    execute_process(COMMAND git rev-parse HEAD
      WORKING_DIRECTORY ${sp_source_dir}/${project}
      OUTPUT_VARIABLE ${project}_revision
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    file(APPEND ${output}
      "${project} commit : ${${project}_revision} (git)<br/>"
      )
    

## #############################################################################
## Svn revision 
## #############################################################################

  elseif(EXISTS ${sp_source_dir}/${project}/.svn)
    execute_process(COMMAND svnversion
      WORKING_DIRECTORY ${sp_source_dir}/${project}
      OUTPUT_VARIABLE ${project}_revision
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    file(APPEND ${output}
    "${project} revision : ${${project}_revision} (svn)<br/>"
    )
  endif()
  
endforeach()

endmacro()
