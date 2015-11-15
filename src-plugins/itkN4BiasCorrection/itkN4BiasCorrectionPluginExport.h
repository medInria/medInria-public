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
    #ifdef itkN4BiasCorrectionPlugin_EXPORTS
        #define ITKN4BIASCORRECTIONPLUGIN_EXPORT __declspec(dllexport) 
    #else
        #define ITKN4BIASCORRECTIONPLUGIN_EXPORT __declspec(dllimport) 
    #endif
#else
    #define ITKN4BIASCORRECTIONPLUGIN_EXPORT
#endif


