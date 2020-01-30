/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
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
    Q_UNUSED(peerTitle);
    Q_UNUSED(peerIP);
    Q_UNUSED(peerPort);
    Q_UNUSED(ourTitle);
    Q_UNUSED(ourIP);
    Q_UNUSED(ourPort);

    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

void medAbstractPacsMoveScu::useBuildInStoreSCP( bool flag )
{
    Q_UNUSED(flag);

    DTK_DEFAULT_IMPLEMENTATION;
}

bool medAbstractPacsMoveScu::setStorageDirectory( const char* directory )
{
    Q_UNUSED(directory);

    DTK_DEFAULT_IMPLEMENTATION;

    return false;
}

bool medAbstractPacsMoveScu::addRequestToQueue( int group, int elem, const char* query, medAbstractPacsNode& moveSource, medAbstractPacsNode& moveTarget)
{
    Q_UNUSED(group);
    Q_UNUSED(elem);
    Q_UNUSED(query);
    Q_UNUSED(moveSource);
    Q_UNUSED(moveTarget);

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
