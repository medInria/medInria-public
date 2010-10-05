#include "dcmtkNode.h"



void dcmtkNode::addConnData( dcmtkConnectionData cdata )
{
    m_connData = cdata;
}

dcmtkNode::dcmtkConnectionData dcmtkNode::getConnData()
{
    return m_connData;
}

dcmtkContainer<dcmtkResultDataset*>* dcmtkNode::getResultDatasetContainer()
{
    return &m_dsContainer;
}
