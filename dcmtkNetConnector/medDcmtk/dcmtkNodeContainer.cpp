#include "dcmtkNodeContainer.h"


void DatasetContainer::clear()
{
    m_findDs.clear();
}

DatasetContainer::~DatasetContainer()
{
    clear();
}

dcmtkFindDataset* DatasetContainer::getFirst()
{
    m_findIter = m_findDs.begin();
    if (m_findIter != m_findDs.end())
        return &(*m_findIter);
    else
        return NULL;
}

dcmtkFindDataset* DatasetContainer::getNext()
{
    m_findIter++;
    if (m_findIter != m_findDs.end())
        return &(*m_findIter);
    else
        return NULL;
}

dcmtkFindDataset* DatasetContainer::getAtPos(unsigned int position)
{
    if (position < m_findDs.capacity())
        return &(m_findDs.at(position));
    else
        return NULL;
}

dcmtkNodeContainer::~dcmtkNodeContainer()
{
   clear();
}

void dcmtkNodeContainer::clear()
{
    m_nodeArr.clear();
}

Node* dcmtkNodeContainer::getFirst()
{
    m_nodeIter = m_nodeArr.begin();
    if (m_nodeIter != m_nodeArr.end())
        return &(*m_nodeIter);
    else
        return NULL;
}

Node* dcmtkNodeContainer::getNext()
{
    m_nodeIter++;
    if (m_nodeIter != m_nodeArr.end())
        return &(*m_nodeIter);
    else
        return NULL;
}

Node* dcmtkNodeContainer::getAtPos(unsigned int position)
{
    if (position < m_nodeArr.capacity())
        return &(m_nodeArr.at(position));
    else
        return NULL;
}