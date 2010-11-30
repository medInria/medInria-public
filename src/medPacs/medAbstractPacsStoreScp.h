
#ifndef MEDABSTRACTPACSSTORESCP_H
#define MEDABSTRACTPACSSTORESCP_H

#include <QtCore/QObject>
#include "medPacsExport.h"

class MEDPACS_EXPORT medAbstractPacsStoreScp : public QObject
{
    Q_OBJECT

public:
    medAbstractPacsStoreScp(void);
    virtual ~medAbstractPacsStoreScp(void);

    virtual  int start(const char* ourTitle, const char* ourIP, unsigned int ourPort);

    virtual  void stop();

    virtual  bool setStorageDirectory(const char* directory);

signals:
    void endOfStudy(QString);

};

#endif
