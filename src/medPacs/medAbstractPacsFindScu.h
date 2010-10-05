
#ifndef MEDABSTRACTPACSFINDSCU_H
#define MEDABSTRACTPACSFINDSCU_H

#include "medAbstractPacsBaseScu.h"
#include "medPacsExport.h"

class MEDPACS_EXPORT medAbstractPacsFindScu : public medAbstractPacsBaseScu
{
    Q_OBJECT

public:
             medAbstractPacsFindScu(void);
    virtual ~medAbstractPacsFindScu(void);

    virtual int  sendFindRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort);

};

#endif
