#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#ifdef WIN32
    #ifdef medSQLitePlugin_EXPORTS
        #define MEDSQLITEPLUGIN_EXPORT __declspec(dllexport) 
    #else
        #define MEDSQLITEPLUGIN_EXPORT __declspec(dllimport) 
    #endif
#else
    #define MEDSQLITEPLUGIN_EXPORT
#endif
