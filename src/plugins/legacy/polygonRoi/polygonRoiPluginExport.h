/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#ifndef POLYGONROIPLUGINEXPORT_H
#define POLYGONROIPLUGINEXPORT_H

#ifdef WIN32
    #ifdef polygonRoiPlugin_EXPORTS
        #define POLYGONROIPLUGIN_EXPORT __declspec(dllexport) 
    #else
        #define POLYGONROIPLUGIN_EXPORT __declspec(dllimport) 
    #endif
#else
    #define POLYGONROIPLUGIN_EXPORT
#endif

#endif
