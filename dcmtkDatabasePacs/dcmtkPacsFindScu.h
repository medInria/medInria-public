#ifndef DCMTKPACSFINDSCU_H
#define DCMTKPACSFINDSCU_H

#include "dcmtkFindScu.h"
#include <medPacs/medAbstractPacsFindScu.h>

class dcmtkPacsFindScu : public medAbstractPacsFindScu, public dcmtkFindScu 
{
        Q_OBJECT

public:
    virtual QString description(void) const;
    static bool     registered(void);

};

medAbstractPacsFindScu *createDcmtkFindScu(void); 

#endif DCMTKPACSFINDSCU_H