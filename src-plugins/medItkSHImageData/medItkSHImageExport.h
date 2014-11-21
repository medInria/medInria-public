/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#ifdef WIN32
    #ifdef medItkSHImagePlugin_EXPORTS
        #define MEDITKSHIMAGEPLUGIN_EXPORT __declspec(dllexport)
    #else
        #define MEDITKSHIMAGEPLUGIN_EXPORT __declspec(dllimport)
    #endif
#else
    #define MEDITKSHIMAGEPLUGIN_EXPORT
#endif
