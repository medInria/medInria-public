#ifndef DCMTKPACSSTORESCP_H
#define DCMTKPACSSTORESCP_H

#include "dcmtkStoreScp.h"

#include <medPacs/medAbstractPacsStoreScp.h>



class dcmtkPacsStoreScp : public medAbstractPacsStoreScp
{
    Q_OBJECT

public:
    virtual QString description(void) const;
    static bool     registered(void);

    int start(const char* ourTitle, const char* ourIP, unsigned int ourPort);


private:
    dcmtkStoreScp   scp;
};

medAbstractPacsStoreScp *createDcmtkStoreScp(void); 

#endif 
