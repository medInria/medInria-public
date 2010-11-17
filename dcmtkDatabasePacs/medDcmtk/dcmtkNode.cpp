#include "dcmtkNode.h"

dcmtkContainer<dcmtkResultDataset*>* dcmtkNode::getResultDatasetContainer()
{
    return &m_dsContainer;
}

dcmtkNode::dcmtkNode( dcmtkNode& node )
{
    setIp(node.ip());
    setTitle(node.title());
    setPort(node.port());
}

