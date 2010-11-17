#include "dcmtkPacsNode.h"

#include "dcmtkPacsResultDataset.h"
#include "dcmtkContainer.h"

#include "medPacs/medAbstractPacsResultDataset.h"

#include <dtkCore/dtkGlobal.h>

QVector<medAbstractPacsResultDataset*> dcmtkPacsNode::getResultDatasetContainer(void)
{
    return m_copyCont;
}

void dcmtkPacsNode::convert( dcmtkNode* node)
{
    this->setTitle(QString::fromStdString(node->title()));
    this->setIp(QString::fromStdString(node->ip()));
    this->setPort(node->port());

    dcmtkContainer<dcmtkResultDataset*>* container = node->getResultDatasetContainer();

    for (int i = 0; i<container->size(); i++) {
        dcmtkPacsResultDataset* ds = new dcmtkPacsResultDataset;
        ds->convert(container->getAtPos(i));
        m_copyCont<<ds;
    }
}
