/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractPacsFindScu.h"

#include <dtkCore/dtkGlobal.h>

medAbstractPacsFindScu::medAbstractPacsFindScu(void)
{

}

medAbstractPacsFindScu::~medAbstractPacsFindScu(void)
{

}
    
int medAbstractPacsFindScu::sendFindRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort)
{
    DTK_UNUSED(peerTitle);
    DTK_UNUSED(peerIP);
    DTK_UNUSED(peerPort);
    DTK_UNUSED(ourTitle);
    DTK_UNUSED(ourIP);
    DTK_UNUSED(ourPort);
    
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}




