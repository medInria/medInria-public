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

QVector<medAbstractPacsResultDataset*>* dcmtkPacsNode::getResultDatasetContainer()
{
    return reinterpret_cast<QVector<medAbstractPacsResultDataset*>*>(&m_dsContainer);
}

void dcmtkPacsNode::convert( dcmtkNode* node)
{
    dcmtkContainer<dcmtkResultDataset*>* container = node->getResultDatasetContainer();
    for (int i= container->size(); i<0; --i)
    {
        dcmtkPacsResultDataset* ds = new dcmtkPacsResultDataset;
        ds->convert(container->getAtPos(i));
        m_dsContainer.push_back(ds);
    }
}
