#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKPROCESSREGISTRATIONOPTIMUSPLUGINEXPORT_H
#define ITKPROCESSREGISTRATIONOPTIMUSPLUGINEXPORT_H

#ifdef WIN32
    #ifdef itkProcessRegistrationOptimusPlugin_EXPORTS
        #define ITKPROCESSREGISTRATIONOPTIMUSPLUGIN_EXPORT __declspec(dllexport) 
    #else
        #define ITKPROCESSREGISTRATIONOPTIMUSPLUGIN_EXPORT __declspec(dllimport) 
    #endif
#else
    #define ITKPROCESSREGISTRATIONOPTIMUSPLUGIN_EXPORT __attribute__ ((visibility ("default")))
#endif

#endif
