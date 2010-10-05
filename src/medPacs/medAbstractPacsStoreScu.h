
#ifndef MEDABSTRACTPACSSTORESCP_H
#define MEDABSTRACTPACSSTORESCP_H

#include <QtCore/QObject>
#include "medPacsExport.h"

class MEDPACS_EXPORT medAbstractPacsStoreScu : public QObject
{
    Q_OBJECT

public:
    medAbstractPacsStoreScu(void);
    virtual ~medAbstractPacsStoreScu(void);

    virtual int  sendStoreRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort);

};

#endif
