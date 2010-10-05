#include "dcmtkPacsFindScu.h"
#include "medPacs/medAbstractPacsFindScu.h"
#include "medPacs/medAbstractPacsFactory.h"

QString dcmtkPacsFindScu::description(void) const
{
    return "dcmtkFindScu";
}

bool dcmtkPacsFindScu::registered(void)
{
    return medAbstractPacsFactory::instance()->registerFindScuType("dcmtkFindScu", createDcmtkFindScu);
}

int dcmtkPacsFindScu::sendFindRequest( const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort )
{
    return scu.sendFindRequest(peerTitle, peerIP, peerPort, ourTitle, ourIP, ourPort);
}

bool dcmtkPacsFindScu::addQueryAttribute(int group, int elem, const char* value)
{
    return scu.addQueryAttribute(group,elem, value);
}

void dcmtkPacsFindScu::setQueryLevel( eQueryLevel level )
{
    scu.setQueryLevel(static_cast<dcmtkBaseScu::eQueryLevel>(level));
}

void dcmtkPacsFindScu::clearAllQueryAttributes()
{
    scu.clearAllQueryAttributes();
}

medAbstractPacsFindScu *createDcmtkFindScu(void) 
{
    return new dcmtkPacsFindScu;
}