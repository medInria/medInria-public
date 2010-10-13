
#ifndef DCMTKPACSSTORESCU_H
#define DCMTKPACSSTORESCU_H

#include "dcmtkStoreScu.h"

#include <medPacs/medAbstractPacsStoreScu.h>

class medAbstractPacsStoreScu;

class dcmtkPacsStoreScu : public medAbstractPacsStoreScu
{
    Q_OBJECT

public:
    virtual QString description(void) const;
    static bool     registered(void);

    int  sendStoreRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, 
                          const char* ourTitle, const char* ourIP, unsigned int ourPort);


private:

    dcmtkPacsStoreScu* scu;

};

medAbstractPacsStoreScu *createDcmtkStoreScu(void); 

#endif 