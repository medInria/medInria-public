#ifndef DCMTKPACSFINDSCU_H
#define DCMTKPACSFINDSCU_H

#include "dcmtkFindScu.h"
#include <medPacs/medAbstractPacsFindScu.h>

class dcmtkPacsFindScu : public medAbstractPacsFindScu
{
        Q_OBJECT

public:
    virtual QString description(void) const;
    static bool     registered(void);

    int sendFindRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, 
                        const char* ourTitle, const char* ourIP, unsigned int ourPort);


    bool addQueryAttribute(int group, int elem, const char* value);

    void setQueryLevel(eQueryLevel level);

    void clearAllQueryAttributes();

private:
    dcmtkFindScu scu;

};

medAbstractPacsFindScu *createDcmtkFindScu(void); 

#endif DCMTKPACSFINDSCU_H