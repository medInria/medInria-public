#ifndef DCMTKPACSNODE_H
#define DCMTKPACSNODE_H


#include <QVector>
#include "dcmtkNode.h"
#include <medPacs/medAbstractPacsNode.h>

class medAbstractPacsResultDatset;
class dcmtkPacsResultDataset;

class dcmtkPacsNode : public medAbstractPacsNode
{
    Q_OBJECT

public:

    struct ConnData
    {
        std::string title;
        std::string ip;
        unsigned int port;
    };

    void convert( dcmtkNode* node);

    void addConnData(ConnData cdata);

    ConnData getConnData();

    QVector<medAbstractPacsResultDataset*>* getResultDatasetContainer();


private:
    ConnData                                   m_connData;
    QVector<dcmtkPacsResultDataset*>           m_dsContainer;

};


#endif DCMTKPACSFINDSCU_H