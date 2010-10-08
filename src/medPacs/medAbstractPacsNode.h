#ifndef MEDABSTRACTPACSNODE_H
#define MEDABSTRACTPACSNODE_H

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

    virtual std::string title(void) {
        return "default title";
    }

    virtual std::string ip(void) {
         return "default ip";
    }
    
    virtual unsigned int port(void) {
        return 22;
    }

    virtual QVector<medAbstractPacsResultDataset*> getResultDatasetContainer();
};

#endif
