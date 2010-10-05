#ifndef DCMTKPACSFINDSCU_H
#define DCMTKPACSFINDSCU_H

#include "dcmtkFindScu.h"
#include <medPacs/medAbstractPacsFindScu.h>
#include "dcmtkPacsNode.h"

class medAbstractPacsNode;

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

    virtual QVector<medAbstractPacsNode*>& getNodeContainer(void);


private:
    dcmtkPacsNode convHelper;
    dcmtkFindScu scu;
    QVector<medAbstractPacsNode*> m_copyCont;
};

medAbstractPacsFindScu *createDcmtkFindScu(void); 

#endif DCMTKPACSFINDSCU_H