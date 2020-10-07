/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifdef WIN32
    #ifdef medComposer_EXPORTS
        #define MEDCOMPOSER_EXPORT __declspec(dllexport)
    #else
        #define MEDCOMPOSER_EXPORT __declspec(dllimport)
    #endif
#else
    #define MEDCOMPOSER_EXPORT
#endif
