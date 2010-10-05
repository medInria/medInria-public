#include "dcmtkPacsNode.h"

#include "dcmtkPacsResultDataset.h"
#include "dcmtkContainer.h"

#include "medPacs/medAbstractPacsResultDataset.h"


void dcmtkPacsNode::addConnData( ConnData cdata )
{
    m_connData = cdata;
}

dcmtkPacsNode::ConnData dcmtkPacsNode::getConnData()
{
    return m_connData;
}

QVector<medAbstractPacsResultDataset*>& dcmtkPacsNode::getResultDatasetContainer()
{
    return m_copyCont;
}

void dcmtkPacsNode::convert( dcmtkNode* node)
{
    m_connData.title = node->getConnData().title;
    m_connData.ip = node->getConnData().ip;
    m_connData.port = node->getConnData().port;


    dcmtkContainer<dcmtkResultDataset*>* container = node->getResultDatasetContainer();
    for (int i = 0; i<container->size(); i++)
    {
        dcmtkPacsResultDataset* ds = new dcmtkPacsResultDataset;
        ds->convert(container->getAtPos(i));
        m_copyCont<<ds;
    }
}
