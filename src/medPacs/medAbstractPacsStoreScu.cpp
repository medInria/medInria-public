#include "medAbstractPacsStoreScu.h"

#include <dtkCore/dtkGlobal.h>

#include <QtCore>

medAbstractPacsStoreScu::medAbstractPacsStoreScu( void )
{

}

medAbstractPacsStoreScu::~medAbstractPacsStoreScu( void )
{

}

int medAbstractPacsStoreScu::sendStoreRequest( const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort )
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
