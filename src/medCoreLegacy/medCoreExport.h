/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifdef WIN32
    #ifdef medCore_EXPORTS
        #define MEDCORE_EXPORT __declspec(dllexport) 
    #else
        #define MEDCORE_EXPORT __declspec(dllimport) 
    #endif
#else
    #define MEDCORE_EXPORT	
#endif
