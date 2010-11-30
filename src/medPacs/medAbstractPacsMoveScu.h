#ifndef MEDABSTRACTPACSMOVESCU_H
#define MEDABSTRACTPACSMOVESCU_H

#include "medAbstractPacsBaseScu.h"
#include "medPacsExport.h"

class medAbstractPacsMoveScu;

class MEDPACS_EXPORT medAbstractPacsMoveScu : public medAbstractPacsBaseScu
{
    Q_OBJECT

public:
             medAbstractPacsMoveScu(void);
    virtual ~medAbstractPacsMoveScu(void);

    virtual int sendMoveRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, 
                                const char* ourTitle, const char* ourIP, unsigned int ourPort);

    virtual void sendCancelRequest();

    virtual void useBuildInStoreSCP(bool flag);

    virtual bool setStorageDirectory(const char* directory);

    virtual bool addRequestToQueue(int group, int elem, const char* query, medAbstractPacsNode& moveSource, medAbstractPacsNode& moveTarget);

    virtual int performQueuedMoveRequests();

signals:
    void progressed(int);

};

#endif
