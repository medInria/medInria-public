#ifndef DCMTKPACSKEY_H
#define DCMTKPACSKEY_H


#include <QVector>
#include "dcmtkKey.h"
#include <medPacs/medAbstractPacsKey.h>



class dcmtkPacsKey : public medAbstractPacsKey
{
    Q_OBJECT

public:


    void convert( dcmtkKey* key);


private:
    
   

};


#endif //DCMTKPACSKEY_H
