#include "dcmtkPacsFindScu.h"
#include "medPacs/medAbstractPacsFindScu.h"
#include "medPacs/medAbstractPacsFactory.h"

bool dcmtkPacsFindScu::registered(void)
{
    return medAbstractPacsFactory::instance()->registerFindScuType("dcmtkFindScu", createDcmtkFindScu);
}

QString dcmtkPacsFindScu::description(void) const
{
    return "dcmtkFindScu";
}

medAbstractPacsFindScu *createDcmtkFindScu(void) 
{
    return new dcmtkPacsFindScu;
}