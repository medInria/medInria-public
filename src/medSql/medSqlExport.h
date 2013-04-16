/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifdef WIN32
    #ifdef medSql_EXPORTS
        #define MEDSQL_EXPORT __declspec(dllexport) 
    #else
        #define MEDSQL_EXPORT __declspec(dllimport) 
    #endif
#else
    #define MEDSQL_EXPORT	
#endif
