/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#ifdef WIN32
    #ifdef meshMappingPlugin_EXPORTS
        #define MESHMAPPINGPLUGIN_EXPORT __declspec(dllexport) 
    #else
        #define MESHMAPPINGPLUGIN_EXPORT __declspec(dllimport) 
    #endif
#else
    #define MESHMAPPINGPLUGIN_EXPORT
#endif


