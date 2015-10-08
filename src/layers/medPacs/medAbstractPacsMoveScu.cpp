/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPacsMoveScu.h>
#include <medAbstractPacsNode.h>

#include <dtkCoreSupport/dtkGlobal.h>

int medAbstractPacsMoveScu::sendMoveRequest( const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort )
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

medAbstractPacsMoveScu::medAbstractPacsMoveScu( void )
{

}

medAbstractPacsMoveScu::~medAbstractPacsMoveScu( void )
{

}

void medAbstractPacsMoveScu::useBuildInStoreSCP( bool flag )
{
    DTK_UNUSED(flag);

    DTK_DEFAULT_IMPLEMENTATION;
}

bool medAbstractPacsMoveScu::setStorageDirectory( const char* directory )
{
    DTK_UNUSED(directory);

    DTK_DEFAULT_IMPLEMENTATION;

    return false;
}

bool medAbstractPacsMoveScu::addRequestToQueue( int group, int elem, const char* query, medAbstractPacsNode& moveSource, medAbstractPacsNode& moveTarget)
{
    DTK_UNUSED(group);
    DTK_UNUSED(elem);
    DTK_UNUSED(query);
    DTK_UNUSED(moveSource);
    DTK_UNUSED(moveTarget);

    DTK_DEFAULT_IMPLEMENTATION;

    return false;
}

int medAbstractPacsMoveScu::performQueuedMoveRequests()
{
    DTK_DEFAULT_IMPLEMENTATION;
    return 0;
}

void medAbstractPacsMoveScu::sendCancelRequest()
{
    DTK_DEFAULT_IMPLEMENTATION;
}
