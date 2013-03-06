#pragma once

#include <QtCore/QObject>
#include <QVector>
#include "medPacsExport.h"

class medAbstractPacsResultDataset;

class MEDPACS_EXPORT medAbstractPacsNode : public QObject
{
    Q_OBJECT
    
public:
             medAbstractPacsNode(void);
    virtual ~medAbstractPacsNode(void);

    virtual void setTitle(QString title);
    
    virtual void setIp(QString ip);

    virtual void setPort(unsigned int port);

    virtual QString title(void) {
        return "default title";
    }

    virtual QString ip(void) {
         return "default ip";
    }
    
    virtual unsigned int port(void) {
        return 22;
    }

    virtual QVector<medAbstractPacsResultDataset*> getResultDatasetContainer();
};


