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
    #ifdef medRemeshingPlugin_EXPORTS
        #define MEDREMESHINGPLUGIN_EXPORT __declspec(dllexport)
    #else
        #define MEDREMESHINGPLUGIN_EXPORT __declspec(dllimport)
    #endif
#else
    #define MEDREMESHINGPLUGIN_EXPORT
#endif


