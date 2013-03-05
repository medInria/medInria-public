#include "medAbstractPacsStoreScp.h"

#include <dtkCore/dtkGlobal.h>

#include <QtCore>

medAbstractPacsStoreScp::medAbstractPacsStoreScp()
{

}

medAbstractPacsStoreScp::~medAbstractPacsStoreScp()
{

}

int medAbstractPacsStoreScp::start( const char* ourTitle, const char* ourIP, unsigned int ourPort )
{
    DTK_UNUSED(ourTitle);
    DTK_UNUSED(ourIP);
    DTK_UNUSED(ourPort);

    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

void medAbstractPacsStoreScp::stop()
{
    DTK_DEFAULT_IMPLEMENTATION;
}

bool medAbstractPacsStoreScp::setStorageDirectory( const char* directory )
{
    DTK_UNUSED(directory);
    return false;

}
