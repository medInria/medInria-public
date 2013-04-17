/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#if defined(WIN32)
    #ifdef ITKDCMTKIO_EXPORTS
        #define ITKDCMTKIO_EXPORT __declspec(dllexport) 
    #else
        #define ITKDCMTKIO_EXPORT __declspec(dllimport) 
    #endif
#else
    #define ITKDCMTKIO_EXPORT 
#endif


