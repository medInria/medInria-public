#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#ifdef WIN32
    #ifdef medPython_EXPORTS
        #define MEDPYTHON_EXPORT __declspec(dllexport)
    #else
        #define MEDPYTHON_EXPORT __declspec(dllimport)
    #endif
#else
    #define MEDPYTHON_EXPORT
#endif
