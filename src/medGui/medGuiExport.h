/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifdef WIN32
    #ifdef medGui_EXPORTS
        #define MEDGUI_EXPORT __declspec(dllexport) 
    #else
        #define MEDGUI_EXPORT __declspec(dllimport) 
    #endif
#else
    #define MEDGUI_EXPORT	
#endif
