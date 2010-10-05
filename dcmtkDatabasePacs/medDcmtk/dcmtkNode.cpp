#include "dcmtkNode.h"

dcmtkContainer<dcmtkResultDataset*>* dcmtkNode::getResultDatasetContainer()
{
    return &m_dsContainer;
}
