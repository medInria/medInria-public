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
    #ifdef medTemporaryDataSourcePlugin_EXPORTS
        #define MEDTEMPORARYDATASOURCEPLUGIN_EXPORT __declspec(dllexport)
    #else
        #define MEDTEMPORARYDATASOURCEPLUGIN_EXPORT __declspec(dllimport)
    #endif
#else
    #define MEDTEMPORARYDATASOURCEPLUGIN_EXPORT
#endif
