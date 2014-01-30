/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#ifdef WIN32
    #ifdef medVtkViewPlugin_EXPORTS
        #define MEDVTKVIEWPLUGIN_EXPORT __declspec(dllexport)
    #else
        #define MEDVTKVIEWPLUGIN_EXPORT __declspec(dllimport)
    #endif
#else
    #define MEDVTKVIEWPLUGIN_EXPORT
#endif



