#ifndef DCMTKPACSECHOSCU_H
#define DCMTKPACSECHOSCU_H

#include "dcmtkEchoScu.h"
#include <medPacs/medAbstractPacsEchoScu.h>

class dcmtkPacsEchoScu : public medAbstractPacsEchoScu
{
    Q_OBJECT

public:
    virtual QString description(void) const;

    static bool registered(void);

    int sendEchoRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, 
                        const char* ourTitle, const char* ourIP, unsigned int ourPort);

private:
    dcmtkEchoScu scu;

};

medAbstractPacsEchoScu *createDcmtkEchoScu(void); 

#endif //DCMTKPACSECHOSCU_H
