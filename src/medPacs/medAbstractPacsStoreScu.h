
#ifndef MEDABSTRACTPACSSTORESCU_H
#define MEDABSTRACTPACSSTORESCU_H

#include <QtCore/QObject>
#include "medPacsExport.h"
#include "medAbstractPacsBaseScu.h"

class MEDPACS_EXPORT medAbstractPacsStoreScu : public medAbstractPacsBaseScu
{
    Q_OBJECT

public:
    medAbstractPacsStoreScu(void);
    virtual ~medAbstractPacsStoreScu(void);

    virtual int  sendStoreRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort);

};

#endif
