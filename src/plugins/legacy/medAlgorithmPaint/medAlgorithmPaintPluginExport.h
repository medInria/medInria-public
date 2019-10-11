/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#ifdef WIN32
   #ifdef medAlgorithmPaint_EXPORTS
       #define MEDALGORITMPAINT_EXPORT __declspec(dllexport)
   #else
       #define MEDALGORITMPAINT_EXPORT __declspec(dllimport)
   #endif
#else
   #define MEDALGORITMPAINT_EXPORT
#endif
