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
 #if defined (medVtkInria_EXPORTS)
  #define MEDVTKINRIA_EXPORT __declspec( dllexport )
 #else
  #define MEDVTKINRIA_EXPORT __declspec( dllimport )
 #endif
#else
    #define MEDVTKINRIA_EXPORT
#endif



