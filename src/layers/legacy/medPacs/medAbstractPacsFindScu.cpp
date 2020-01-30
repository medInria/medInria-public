/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPacsFindScu.h>

#include <dtkCoreSupport/dtkGlobal.h>

int medAbstractPacsFindScu::sendFindRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort)
{
    Q_UNUSED(peerTitle);
    Q_UNUSED(peerIP);
    Q_UNUSED(peerPort);
    Q_UNUSED(ourTitle);
    Q_UNUSED(ourIP);
    Q_UNUSED(ourPort);
    
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}




