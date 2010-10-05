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

    struct ConnData
    {
        std::string title;
        std::string ip;
        unsigned int port;
    };

    void addConnData(ConnData cdata);

    ConnData getConnData();

    QVector<medAbstractPacsResultDataset*>& getResultDatasetContainer();

};

#endif