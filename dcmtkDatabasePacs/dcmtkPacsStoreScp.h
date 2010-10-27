#ifndef DCMTKPACSSTORESCP_H
#define DCMTKPACSSTORESCP_H

#include "dcmtkStoreScp.h"

#include <medPacs/medAbstractPacsStoreScp.h>



class dcmtkPacsStoreScp : public medAbstractPacsStoreScp
{
    Q_OBJECT

public:
                    dcmtkPacsStoreScp();

    virtual QString description(void) const;
    static bool     registered(void);

    int start(const char* ourTitle, const char* ourIP, unsigned int ourPort);

    void stop();

    bool setStorageDirectory(const char* directory);


private:
    dcmtkStoreScp   scp;
};

medAbstractPacsStoreScp *createDcmtkStoreScp(void); 

#endif 
