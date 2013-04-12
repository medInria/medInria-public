#pragma once

#include "medAbstractPacsBaseScu.h"
#include "medPacsExport.h"

class MEDPACS_EXPORT medAbstractPacsEchoScu : public medAbstractPacsBaseScu
{
    Q_OBJECT

public:
             medAbstractPacsEchoScu();
    virtual ~medAbstractPacsEchoScu();

    virtual int  sendEchoRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, 
                                 const char* ourTitle, const char* ourIP, unsigned int ourPort);

};


