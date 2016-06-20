/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifdef WIN32
    #ifdef medComposerImagingExtension_EXPORTS
        #define MEDCOMPOSERIMAGINGEXTENSION_EXPORT __declspec(dllexport)
    #else
        #define MEDCOMPOSERIMAGINGEXTENSION_EXPORT __declspec(dllimport)
    #endif
#else
    #define MEDCOMPOSERIMAGINGEXTENSION_EXPORT
#endif
