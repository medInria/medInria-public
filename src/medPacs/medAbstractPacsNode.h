#pragma once

#include <QtCore/QObject>
#include <QVector>
#include "medPacsExport.h"

class medAbstractPacsResultDataset;

class MEDPACS_EXPORT medAbstractPacsNode : public QObject
{
    Q_OBJECT
    
public:
             medAbstractPacsNode();
    virtual ~medAbstractPacsNode();

    virtual void setTitle(QString title);
    
    virtual void setIp(QString ip);

    virtual void setPort(unsigned int port);

    virtual QString title() {
        return "default title";
    }

    virtual QString ip() {
         return "default ip";
    }
    
    virtual unsigned int port() {
        return 22;
    }

    virtual QVector<medAbstractPacsResultDataset*> getResultDatasetContainer();
};


