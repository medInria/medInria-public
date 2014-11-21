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
    #ifdef medItkDiffusionGradientListPlugin_EXPORTS
        #define medItkDiffusionGradientListPlugin_EXPORT __declspec(dllexport) 
    #else
        #define medItkDiffusionGradientListPlugin_EXPORT __declspec(dllimport) 
    #endif
#else
    #define medItkDiffusionGradientListPlugin_EXPORT
#endif


